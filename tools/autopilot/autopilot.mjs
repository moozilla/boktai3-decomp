#!/usr/bin/env node
// The Autopilot: plays Boktai 3 on the source port from typed game state.
//
//   node tools/autopilot/autopilot.mjs m1 [--save F] [--out DIR] [--shots]
//
// Runs headless in-process (port/agent/session.mjs) with the agent library plus
// tools/autopilot/lib/*.js. Each milestone is a goal; the run writes the input
// movie (movie.json: key mask per frame, and per input read), the same inputs as a
// tools/emu/harness script keyed to game ticks (harness.txt), and a log of what each
// skill did (log.json). --shots writes a PNG at each event into DIR/shots/;
// --verify replays harness.txt on baserom.gba and compares where it ends.
//
// Milestones:
//   m1  power-on -> sensor, clock, region, title, new game, password prompt,
//       name entry -> intro narration -> the first line of dialogue in the field.
import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';
import { openSession, png } from '../../port/agent/session.mjs';
import { runRom } from './lib/rom.mjs';

const ROOT = path.resolve(path.dirname(new URL(import.meta.url).pathname), '../..');
const args = process.argv.slice(2);
const opt = (name, dflt) => { const i = args.indexOf(name); return i < 0 ? dflt : args[i + 1]; };
const goal = args[0] || 'm1';
const out = path.resolve(opt('--out', path.join(ROOT, 'build/autopilot', goal)));
const shots = args.includes('--shots');
fs.mkdirSync(out, { recursive: true });
if (shots) fs.mkdirSync(path.join(out, 'shots'), { recursive: true });

// --from DIR/movie.json: start where an earlier run ended (its movie is replayed).
const from = opt('--from') ? JSON.parse(fs.readFileSync(opt('--from'), 'utf8')) : null;
const s = await openSession({
  movie: from ? from.movie : undefined, solarLog: from ? from.solar : undefined,
  save: opt('--save') || (from && from.save) || undefined, render: shots, print: () => {},
  lib: [path.join(ROOT, 'port/agent/lib'), path.join(ROOT, 'tools/autopilot/lib')],
});
// Script-derived stage data (doors, entrances, items, enemies): tools/assets/stages.py json.
const STAGES = (() => { try { return JSON.parse(fs.readFileSync(path.join(ROOT, 'build/assets/stages.json'), 'utf8')); } catch { return []; } })();
// What carries which reaction flag (tools/autopilot/breakers.py: Breakable_OnHit, the gun's
// lens table, weapon modifiers, read from the source and the ROM's weapon records).
const BREAKERS = (() => {
  const f = path.join(ROOT, 'build/autopilot/breakers.json');
  if (!fs.existsSync(f)) spawnSync('python3', [path.join(ROOT, 'tools/autopilot/breakers.py'), f]);
  try { return JSON.parse(fs.readFileSync(f, 'utf8')); } catch { return { targets: { Breakable: {} }, lens: [], weapons: [] }; }
})();
// means(flag, bags): the lenses and swords that carry the flag, owned first
const means = (flag, bags) => [
  ...BREAKERS.lens.filter((l) => l.flags & flag).map((l) => ({ kind: 'lens', id: l.id, name: l.name, owned: bags.lenses.includes(l.id) })),
  ...BREAKERS.weapons.filter((w) => w.flags & flag).map((w) => ({ kind: 'sword', id: w.id, name: w.name, owned: bags.swords.includes(w.id) })),
].sort((a, b) => b.owned - a.owned);
const stageOf = (scene) => STAGES.find((x) => x.scene === scene) || { items: [], doors: [], entrances: [] };
const log = [];
// scenes the run has been in (carried over from --from), for door choice
const UNREACHABLE = new Set(from && from.unreachable ? from.unreachable : []);  // 'scene:box' doors no route reached
const SEEN = new Map();  // person id -> rooms they were in
const VISITS = new Map();  // room visits this run: least visited doors first
const TRIED = new Set(from && from.tried ? from.tried : []);  // floor items already gone for
const VISITED = new Set(from ? [...(from.visited || []), ...(from.rooms || []).map((r) => r.scene)] : []);
const event = (what) => {
  const e = { frame: s.frame, ...what };
  log.push(e);
  console.log(JSON.stringify(e));
  if (shots) fs.writeFileSync(path.join(out, 'shots', `${String(s.frame).padStart(6, '0')}.png`), png(s.pixels()));
};

const BORING = /^(InterruptsAndVideo|TaskSystem|PositionQueue|SceneActor|SceneBody|SceneStimulus|SceneMessages|Solar_Update|DelayedScript|ColorFade|SceneLifecycle|ScrollingDialogue|TextWindowManager|MessageWindow_Update)/;
const GOALS = {
  // Power-on to the first message window line after name entry.
  *m1(g, L) {
    const msg = L.message();
    let named = false, seen;
    for (let idle = 0; g.frameNo < 20000;) {
      const step = yield* L.menuStep({ password: 'no' });
      if (step) {
        event({ skill: 'menu', ...step, scene: L.where().scene });
        if (step.menu === 'NameEntry_Update' && step.done) named = true;
        idle = 0;
        continue;
      }
      // The intro narration (scene 6551) uses the message window too; the opening
      // dialogue is the first line once the field has a player.
      const field = L.tasks().some((t) => !t.dead && t.update === 'Player_Update');
      if (named && field && msg.open && msg.id !== undefined && msg.pages > 0) {
        yield 30;  // let the window draw its first page
        event({ skill: 'goal', reached: 'opening dialogue', string: msg.id, scene: L.where().scene,
          anchor: { fn: g.addr('Text_LookupString') & ~1, r0: msg.id } });
        return true;
      }
      if (msg.open && !field && msg.id !== seen) { seen = msg.id; event({ skill: 'narration', string: msg.id, scene: L.where().scene }); }
      yield 1;
      if (++idle % 600 === 0) event({ skill: 'wait', idle, scene: L.where().scene, tasks: [...new Set(L.tasks().filter((t) => !t.dead).map((t) => t.update))].filter((n) => !BORING.test(n)) });
    }
    return false;
  },

  // M1, then the story's dialogue until the player has control (no message for
  // 120 frames in a room with a player), then through a door: box --box (default
  // CD23, the exit of the first corridor, scene 2698 -> 2705).
  *m2(g, L) {
    if (!(yield* GOALS.m1(g, L))) return false;
    const box = opt('--box', 'CD23');
    if (!(yield* untilControl(g, L, () => L.boxes().some((b) => b.name === box)))) return false;
    const w = L.where();
    event({ skill: 'control', ...w, boxes: L.boxes().map((b) => b.name) });
    if (!(yield* throughDoor(g, L, box))) return false;
    yield g.until(() => L.message().open || L.tasks().some((t) => !t.dead && t.update === 'Player_Update' && L.where().scene !== w.scene), 600);
    event({ skill: 'goal', reached: `through door ${box}`, ...L.where() });
    return true;
  },

  // Onward: M1, then repeatedly play to control and leave by a door, preferring
  // scenes not visited yet (stages.json doors, else any box the live map has that a
  // door record names). Logs each stop; --scenes N stops after N rooms.
  *story(g, L) {
    if (!from && !(yield* GOALS.m1(g, L))) return false;
    const visited = VISITED, limit = +(opt('--scenes', 8));
    let prev;
    for (let rooms = 0; rooms < limit; rooms++) {
      const doorsHere = () => (L.hub() ? stageOf(L.where().scene).doors.filter((d) => d.hub !== undefined && (!d.hubMenu || d.hubMenu === L.hub().kind))
          .filter((d, i, all) => all.findIndex((e) => e.hub === d.hub) === i)  // one per entry (its conditional destinations)
        : stageOf(L.where().scene).doors.filter((d) => L.boxes().some((b) => b.name === d.box)));
      if (!(yield* untilControl(g, L, () => doorsHere().length > 0, 120, 30000))) { event({ skill: 'stop', why: 'no control with a door in 30000 frames', ...L.where() }); return rooms > 0; }
      // enemies in the room: fight them first (the "defeat all" rooms lock the doors)
      if (L.enemies().some((e) => e.alive && !(L.fightIgnored && L.fightIgnored.has(e.addr)))) {
        const f = yield* L.fight({ trace: (e) => event({ skill: 'fight', ...e }) });
        event({ skill: 'fought', ...f, scene: L.where().scene });
        if (!(yield* untilControl(g, L, () => doorsHere().length > 0, 60, 30000))) { event({ skill: 'stop', why: 'no control after the fight', ...L.where() }); return rooms > 0; }
      }
      const here = L.where().scene;
      // healing: below a third of the life bar with something in the item bag, find the
      // use of an item in the pause menu that raises life (menuSearch; once per room)
      {
        const p = $player(g) && g.$.gPlayers[0], life = () => g.$.gPlayers[0] ? g.$.gPlayers[0].currentStatA : 0;
        if (p && p.maximumStatA && p.currentStatA * 3 < p.maximumStatA && L.bags().items.length) {
          const life0 = p.currentStatA, items0 = JSON.stringify(L.bags().items);
          // items page only (L/R over the other pages crashed the port); the life bar fills
          // over many frames, so a used item is judged by the bag changing as well
          const r = yield* L.menuSearch(() => life() > life0 || JSON.stringify(L.bags().items) !== items0,
            { max: 300, page: 1, keys: ['down', 'right', 'up', 'left', 'a', 'b'], settle: 90 });
          if (r.done) yield g.press(0, 120);
          event({ skill: 'heal', from: life0, to: life(), ...r });
        }
      }
      // floor switches (stages.json: a trap on an HP box that runs a script once, e.g. a
      // chest appears): step on each once
      for (const sw of (stageOf(here).switches || []).filter((sw) => !TRIED.has(`${here}:switch:${sw.box}`))) {
        TRIED.add(`${here}:switch:${sw.box}`);
        const r = yield* L.navigate([sw.x, sw.z], { avoid: stageOf(here).doors.map((d) => d.box), trace: (e) => event({ skill: 'nav', ...e }) });
        yield* L.clearInterruptions({});
        event({ skill: 'switch', box: sw.box, reached: r.reached, reason: r.reason });
        if (L.where().scene !== here) break;
      }
      // hittables with a script (lever switches): strike each once
      for (const h of (stageOf(here).hittables || []).filter((h) => h.flags === 9 && !TRIED.has(`${here}:${h.breakable ? 'break' : 'hit'}:${h.name}`))) {
        if (h.breakable) {
          // a Breakable answers only an attack carrying the flag its state needs (a
          // candlestick: Flame); the means: a lens or sword with that flag
          const live = L.breakables().find((b) => b.id === parseInt(h.name, 16));
          const need = live && BREAKERS.targets.Breakable[live.state];
          if (!need) { TRIED.add(`${here}:break:${h.name}`); continue; }
          const ways = means(need.flag, L.bags()), way = ways.find((w) => w.owned && w.kind === 'lens');
          if (!way) { event({ skill: 'needs', name: h.name, state: live.state, flag: need.flag, means: ways.map((w) => `${w.kind} ${w.name}`) }); continue; }
          TRIED.add(`${here}:break:${h.name}`);
          // a lens not on the Quick Change list is put there in the pause menu (Solar Gun page)
          let qc = yield* L.quickChange({ gun: true, lens: way.id });
          if (!qc.done) {
            const m = yield* L.menuSearch(() => L.weapon().lens === way.id, { max: 300 });
            event({ skill: 'menu', goal: `lens ${way.name}`, ...m });
            qc = yield* L.quickChange({ gun: true, lens: way.id });
          }
          const ch = yield* L.chargeEnergy(6, { lux: 0, restore: 232 });
          event({ skill: 'equip', ...way, quickChange: qc, charge: ch });
          const changed = () => { const b = L.breakables().find((x) => x.id === live.id); return !b || b.state !== live.state; };
          let r = { done: false };
          for (const dist of [2, 1, 3]) {
            r = yield* L.hitAt(h.x >> 8, h.z >> 8, { dist, hits: 4, done: changed, stopOnEvent: true, avoid: stageOf(here).doors.map((d) => d.box), trace: (e) => event({ skill: 'nav', ...e }) });
            if (changed() || L.where().scene !== here) break;
          }
          event({ skill: 'hit', name: h.name, x: h.x, z: h.z, with: `${way.kind} ${way.name}`, lit: changed(), ...r });
          yield* untilControl(g, L, () => true, 60, 3000);
          if (L.where().scene === here) yield* L.quickChange({ gun: false });
          if (L.where().scene !== here) break;
          continue;
        }
        TRIED.add(`${here}:hit:${h.name}`);
        const r = yield* L.hitAt(h.x >> 8, h.z >> 8, { hits: h.breakable ? 12 : 3, stopOnEvent: true, avoid: stageOf(here).doors.map((d) => d.box), trace: (e) => event({ skill: 'nav', ...e }) });
        event({ skill: 'hit', name: h.name, x: h.x, z: h.z, ...r });
        if (L.where().scene !== here) break;
      }
      // people: talk to each once (towns count who was heard before the story goes on).
      // The live actors (world.js people(): SceneActorManager) where they stand now, else
      // the characters the script placed (stages.json talk).
      if (!opt('--no-talk')) {
        const live = L.people();
        // someone met in three or more rooms travels with the player (Otenko): talk once
        for (const q of live) { const k = q.id.toString(16); SEEN.set(k, (SEEN.get(k) || new Set()).add(here)); }
        const companion = (key) => (SEEN.get(key) || new Set()).size >= 3;
        const targets = live.length ? live.map((q) => ({ key: q.id.toString(16), x: q.x, z: q.z, addr: q.addr }))
          : (stageOf(here).talk || []).filter((t) => t.kind === 'character').map((t) => ({ key: String(t.id), x: t.x, z: t.z }));
        // twice each (a second talk can say more: San Miguel's flag r708[0x19c.4][6] is set
        // on the second talk in 2370), again when the story counter moved
        const tkey = (t) => `${here}:talk:${t.key}:${L.story()}`;
        for (const t0 of targets.filter((t) => !TRIED.has(tkey(t)))) for (let round = 0; round < 2; round++) {
          const t = t0;
          const story0 = L.story(), now = t.addr ? L.people().find((q) => q.addr === t.addr) : null;
          const r = yield* L.interact(now ? now.x : t.x, now ? now.z : t.z, { avoid: stageOf(here).doors.map((d) => d.box), trace: (e) => event({ skill: 'nav', ...e }) });
          // a shop or service menu the talk opened: back out of it
          for (let i = 0; i < 8 && !$player(g); i++) { yield g.press('b', 2); yield g.press(0, 10); yield* L.clearInterruptions({}); }
          if (round === 0 && r.done) TRIED.add(tkey(t));  // a failed talk is tried again on the next visit
          event({ skill: 'talk', id: t.key, round, x: t.x, z: t.z, done: r.done, reason: r.reason, lines: (r.lines || []).slice(0, 6), story: L.story(), was: story0, facilities: L.facilities() });
          // whatever the talk started (a choice, a menu, an event) ends before the next one
          yield* untilControl(g, L, () => true, 30, 1200);
          if (L.where().scene !== here || !r.done || companion(t.key)) break;
        }
      }
      // items: pick them up on the way (floor items, chests: a sword, fruit, potions)
      for (const it of stageOf(here).items.filter((it) => !TRIED.has(`${here}:${it.x},${it.z}`))) {
        TRIED.add(`${here}:${it.x},${it.z}`);
        const c = yield* L.collect(it.x, it.z, { avoid: stageOf(here).doors.map((d) => d.box), trace: (e) => event({ skill: 'nav', ...e }) });
        event({ skill: 'collect', item: it.name || `${it.category}:${it.item}`, x: it.x, z: it.z, ...c });
        if (c.reason === 'no path') TRIED.delete(`${here}:${it.x},${it.z}`);  // another part of the room: next visit
        if (L.where().scene !== here) break;
      }
      if (L.where().scene !== here) { rooms--; continue; }
      visited.add(here);
      if (prev !== undefined) visited.add(prev);
      // --toward SCENE: a door there first; --goal SCENE: the door on the shortest route
      // over the door graph (stages.json); then unvisited scenes
      const toward = rooms === 0 ? +opt('--toward', -1) : -1;
      // with no --goal: explore, the door on the shortest route (over doors not known to
      // be unreachable) to the nearest scene not visited yet
      const dist = +opt('--goal', -1) >= 0 ? routeDistances(+opt('--goal', -1)) : exploreDistances(here, visited);
      VISITS.set(here, (VISITS.get(here) || 0) + 1);
      const doors = doorsHere().sort((a, b) => (b.scene === toward) - (a.scene === toward) ||
        (dist.get(a.scene) ?? 1e9) - (dist.get(b.scene) ?? 1e9) || visited.has(a.scene) - visited.has(b.scene) ||
        (VISITS.get(a.scene) || 0) - (VISITS.get(b.scene) || 0));
      event({ skill: 'room', ...L.where(), doors: doors.map((d) => `${d.hub !== undefined ? 'hub' + d.hub : d.box}->${d.scene}`) });
      let moved = false;
      // a room can still be playing its arrival event (a dialogue that starts late, the
      // map changing under the player): let it run and try the doors again
      for (let round = 0; round < 4 && !moved; round++) {
        if (round) {
          event({ skill: 'wait', why: 'no door worked yet', round, ...L.where() });
          yield* untilControl(g, L, () => true, 240, 6000);
        }
        for (const d of doors) {
          prev = here;
          if (d.hub !== undefined) {
            VISITS.set(d.scene, (VISITS.get(d.scene) || 0) + 1);
            visited.add(d.scene);
            const r = yield* L.chooseHub(d.hub);
            event({ skill: 'hub', entry: d.hub, to: d.scene, ...r });
            yield g.until(() => L.where().scene !== here, 600);
            if (L.where().scene !== here) { moved = true; break; }
            continue;
          }
          if (yield* throughDoor(g, L, d.box)) { moved = true; break; }
          if (L.where().scene === here) UNREACHABLE.add(`${here}:${d.box}`);
          if (L.where().scene !== here) { moved = true; break; }
        }
      }
      if (!moved) { event({ skill: 'stop', why: 'no door worked', ...L.where() }); return rooms > 0; }
    }
    return true;
  },

  // With --from: nothing more (to verify, cover and save states for an earlier run).
  *replay(g) { yield 1; return true; },

  // With --from: hold --keys (default none) for --frames frames, logging scene changes
  // and life every 300 frames (to see what a section does on its own).
  *hold(g, L) {
    const keys = opt('--keys', '') || 0, n = +opt('--frames', 3000);
    for (let i = 0; i < n; i += 300) {
      yield g.press(keys, 300);
      event({ skill: 'hold', keys, ...L.where(), story: L.story() });
    }
    return true;
  },

  // With --from: navigate to --box from where that run ended (a debugging aid).
  *nav(g, L) {
    yield 1;
    const box = opt('--box', 'CD23');
    return yield* throughDoor(g, L, box);
  },

  // Exploration aid: M1, then advance every message and log scene changes, with a
  // shot at each (and every 1200 idle frames) when --shots is given.
  *explore(g, L) {
    if (!(yield* GOALS.m1(g, L))) return false;
    const msg = L.message(), limit = +(opt('--frames', 30000));
    let scene = L.where().scene, idle = 0;
    while (g.frameNo < limit) {
      const step = yield* L.menuStep({});
      if (step) { event({ skill: 'menu', ...step }); continue; }
      if (msg.open) { yield* L.advanceText(600); event({ skill: 'dialogue', string: msg.id, scene: L.where().scene }); idle = 0; continue; }
      yield 1;
      const w = L.where();
      if (w.scene !== scene) { scene = w.scene; event({ skill: 'scene', ...w }); idle = 0; }
      else if (++idle % 1200 === 0) event({ skill: 'wait', ...w, tasks: [...new Set(L.tasks().filter((t) => !t.dead).map((t) => t.update))].filter((n) => !BORING.test(n)).slice(0, 12) });
    }
    return true;
  },
};

// Exploration distances: breadth first from HERE over stages.json doors (skipping doors a
// run found unreachable from their side) to the nearest unvisited scene; each door's
// destination gets the number of doors from it to that frontier.
function exploreDistances(here, visited) {
  const out = new Map(), adj = new Map();
  for (const st of STAGES) adj.set(st.scene, st.doors.filter((d) => !UNREACHABLE.has(`${st.scene}:${d.box}`)).map((d) => d.scene));
  const toFrontier = new Map();  // scene -> doors to the nearest unvisited scene
  const frontier = STAGES.map((st) => st.scene).filter((sc) => !visited.has(sc));
  // reverse breadth first from all unvisited scenes
  let level = frontier.slice();
  for (const sc of level) toFrontier.set(sc, 0);
  while (level.length) {
    const next = [];
    for (const [from, tos] of adj) if (!toFrontier.has(from) && tos.some((t) => level.includes(t))) { toFrontier.set(from, 1 + Math.min(...tos.filter((t) => level.includes(t)).map((t) => toFrontier.get(t)))); next.push(from); }
    level = next;
  }
  for (const [sc, d] of toFrontier) out.set(sc, d);
  return out;
}

// Door-graph distances to GOAL (breadth first over stages.json doors, walked backwards):
// scene -> number of doors to the goal. Empty without a goal.
function routeDistances(goal) {
  const d = new Map();
  if (goal < 0) return d;
  d.set(goal, 0);
  for (let frontier = [goal]; frontier.length;) {
    const next = [];
    for (const st of STAGES) for (const door of st.doors)
      if (frontier.includes(door.scene) && !d.has(st.scene)) { d.set(st.scene, d.get(door.scene) + 1); next.push(st.scene); }
    frontier = next;
  }
  return d;
}

// Through door box BOX: navigate; when a scripted gate turns the player back, open
// the room's chests and items (the usual ask), then try again.
function* throughDoor(g, L, box) {
  const scene = L.where().scene, tried = new Set();
  let fights = 0;
  for (let attempt = 0; attempt < 6; attempt++) {
    const avoid = stageOf(scene).doors.map((d) => d.box).filter((b) => b !== box);
    // enemies that appear on the way (a scripted ambush, a boss after its banner): fight
    const foes = () => L.enemies().concat(L.targets().filter((t) => t.health !== null)).some((e) => e.alive && !(L.fightIgnored && L.fightIgnored.has(e.addr)));
    const r = yield* L.navigate(box, { avoid, stopIf: foes, trace: (e) => event({ skill: 'nav', ...e }) });
    event({ skill: 'navigate', box, ...r, from: scene });
    if (r.reached) return true;
    if (r.reason === 'stopped' && fights++ < 5) {
      const f = yield* L.fight({ trace: (e) => event({ skill: 'fight', ...e }) });
      event({ skill: 'fought', ...f, scene: L.where().scene });
      attempt--;
      continue;
    }
    if (r.reason !== 'gated') return false;
    const item = stageOf(scene).items.find((it) => !tried.has(`${it.x},${it.z}`));
    if (!item) return false;
    tried.add(`${item.x},${item.z}`);
    const i = yield* L.interact(item.x, item.z, { trace: (e) => event({ skill: 'nav', ...e }) });
    event({ skill: 'interact', item: item.name, x: item.x, z: item.z, ...i });
  }
  return false;
}

// End a run at a stable point (the player still for 30 frames, at most 600), so a
// replay compares positions rather than a frame inside a walk.
function* settle(g) {
  let last = '', still = 0;
  for (let i = 0; i < 600 && still < 30; i++) {
    yield 1;
    const p = g.$.gPlayers[0], k = p ? `${p.position.x},${p.position.z}` : '-';
    still = k === last ? still + 1 : 0;
    last = k;
  }
}

// Advance dialogue (and menus) until the player stands in a room with no message
// open for `quiet` frames (and `ready()` holds, e.g. the room has a given box); at
// most `limit` frames.
function* untilControl(g, L, ready = () => true, quiet = 120, limit = 60000) {
  const msg = L.message(), end = s.frame + limit;
  let stuck = 0;
  for (let calm = 0; s.frame < end;) {
    const step = yield* L.menuStep({});
    if (step) { event({ skill: 'menu', ...step }); calm = 0; continue; }
    if (msg.open) { yield* L.advanceText(600); event({ skill: 'dialogue', string: msg.id, scene: L.where().scene }); calm = 0; continue; }
    yield 1;
    const field = $player(g) || (L.hub() && L.hub().state === 'TownMap_UpdateSelection');
    calm = field && ready() ? calm + 1 : 0;
    if (calm >= quiet) return true;
    // an unknown menu holds the screen (no player, no message, no known menu): back out
    // with B, or START (the bike parts menu closes on START when it was opened from a
    // story scene: ModifierMenu_HandleOverviewInput), now and then confirm with A
    stuck = field ? 0 : stuck + 1;
    // riding the coffin bike (the Glove task; src/npc/sabata_rider/registry_actor_collision.c
    // ActorMotion_ApplyInput: R accelerates, L boosts, up/down steer): ride on with R
    if (!field && !msg.open && L.tasks().some((t) => !t.dead && t.update === 'CoffinBike_UpdateTask')) {
      if (stuck % 600 === 1) event({ skill: 'ride', ...L.where() });
      yield g.press('r', 4);
      continue;
    }
    if (stuck && stuck % 180 === 0) {
      const key = ['b', 'start', 'b', 'start', 'a'][(stuck / 180) % 5];
      event({ skill: 'unstick', key, frames: stuck, tasks: [...new Set(L.tasks().filter((t) => !t.dead).map((t) => t.update))].filter((n) => !BORING.test(n)).slice(0, 8) });
      yield g.press(key, 2);
    }
  }
  return false;
}
const $player = (g) => g.$.gPlayers[0] && g.lib.tasks().some((t) => !t.dead && t.update === 'Player_Update');

if (!GOALS[goal]) { console.error(`unknown goal ${goal} (${Object.keys(GOALS).join(' ')})`); process.exit(2); }
const t0 = performance.now();
// The keys each input read saw (Input_UpdatePadStates runs once per game tick; the
// port has no lag frames, the hardware does, so replays key inputs to these reads).
// readAt[n]: frame n ran an input read (truncated with the movie on a restore).
const readAt = [];
if (from && from.readFrames) for (const n of from.readFrames) readAt[n] = 1;
const snapshot = s.snapshot, restore = s.restore;
s.snapshot = () => ({ ...snapshot(), readAt: readAt.slice() });
s.restore = (snap) => { restore(snap); readAt.length = 0; for (let i = 0; i < snap.movie; i++) readAt[i] = snap.readAt[i]; };
const ok = s.run(function* (g) {
  g.lib.session = s;
  g.on('Input_UpdatePadStates', () => { readAt[s.frame] = 1; });
  // --life N: set the player's life to N before the goal (a port-only test aid, e.g. of
  // healing; such a run cannot be verified on the ROM)
  if (opt('--life') && g.$.gPlayers[0]) g.$.gPlayers[0].currentStatA = +opt('--life');
  const reached = yield* GOALS[goal](g, g.lib);
  yield* settle(g);
  return reached;
});
const secs = (performance.now() - t0) / 1000;
const end = s.run(function* (g) { const w = g.lib.where(); return { scene: w.scene, x: w.x, y: w.y, z: w.z }; });

const movie = s.movie.map((k) => k | 0);
const readFrames = [];
for (let n = 0; n < movie.length; n++) if (readAt[n]) readFrames.push(n);
const reads = readFrames.map((n) => movie[n]);
function readFramesOf() { return readFrames; }
// a savestate on the ROM at each room the run entered: the input read index there
const allEvents = [...(from && from.rooms ? from.rooms : []), ...log.filter((e) => e.skill === 'room' || e.skill === 'goal').map((e) => ({ frame: e.frame, scene: e.scene }))];
const saves = allEvents.map((e) => ({ at: readFrames.filter((n) => n < e.frame).length, name: `scene_${e.scene}_f${e.frame}` }));
fs.writeFileSync(path.join(out, 'movie.json'), JSON.stringify({ goal, save: opt('--save') || (from && from.save) || null, frames: movie.length, end, visited: [...VISITED], tried: [...TRIED], unreachable: [...UNREACHABLE], rooms: allEvents, solar: s.solarLog, readFrames, movie }));
fs.writeFileSync(path.join(out, 'harness.txt'), harnessScript(reads, saves, `autopilot ${goal}: ${ok ? 'reached' : 'NOT reached'}; port end ${JSON.stringify(end)}`));
fs.writeFileSync(path.join(out, 'log.json'), JSON.stringify(log, null, 1));
console.log(`${goal}: ${ok ? 'reached' : 'NOT reached'} at frame ${s.frame} (${secs.toFixed(1)} s), end ${JSON.stringify(end)} -> ${path.relative(ROOT, out)}`);
let verified = true;
if (args.includes('--verify') && opt('--life')) console.log('--verify skipped: --life changed the port\'s memory');
else if (args.includes('--verify')) verified = verify(end);
process.exit(ok && verified ? 0 : 1);

// The run as a tools/emu/harness script for the original ROM: `holdt KEYS N` holds
// keys for N calls of Input_UpdatePadStates (tick), so the ROM sees the same keys
// at the same game ticks however many lag frames it has. Ends with a dump of EWRAM
// (verify reads the scene and player position from it) and a screenshot. Needs the
// harness with tick/holdt (tools/emu/harness.c; `make -C tools/emu harness`).
function harnessScript(reads, saves, title) {
  const NAMES = ['A', 'B', 'SELECT', 'START', 'RIGHT', 'LEFT', 'UP', 'DOWN', 'R', 'L'];
  const tick = s.g.addr('Input_UpdatePadStates') & ~1;
  const time = Math.floor(Date.parse('2025-11-13T12:00:00Z') / 1000);
  const lines = [`# ${title}`, '# generated by tools/autopilot/autopilot.mjs (inputs keyed to game ticks)',
    args.includes('--coverage') ? 'trace on' : 'trace off', `rtc ${time}`, 'lux 232', `tick ${tick.toString(16).toUpperCase()}`];
  // sensor changes (session solar log, by frame) as `lux` lines before the first tick at or after them
  const lux = s.solarLog.map(([f, v]) => ({ at: readFramesOf().filter((n) => n < f).length, lux: v }));
  const marks = [...saves, ...lux].sort((a, b) => a.at - b.at);
  for (let i = 0, m = 0; i < reads.length;) {
    while (m < marks.length && marks[m].at <= i) { const k = marks[m++]; lines.push(k.lux !== undefined ? `lux ${k.lux}` : `save ${k.name}`); }
    let j = i;
    const stop = m < marks.length ? marks[m].at : Infinity;
    while (j < reads.length && reads[j] === reads[i] && j < stop) j++;
    const keys = NAMES.filter((_, b) => reads[i] & (1 << b)).join('+') || 'NONE';
    lines.push(`holdt ${keys} ${j - i}`);
    i = j;
  }
  lines.push('holdt NONE 1', 'dump ewram 02000000:40000', 'dump iwram 03000000:8000', 'shot end');
  return lines.join('\n') + '\n';
}

// Replay harness.txt on baserom.gba and compare the scene and player position
// (gGameState->unk5A4 and the first player's position) with the port's.
function verify(end) {
  const dir = path.join(out, 'rom');
  fs.mkdirSync(dir, { recursive: true });
  try {
    runRom(path.join(out, 'harness.txt'), dir, {
      save: opt('--save') || from?.save, coverage: args.includes('--coverage'), timeout: 900_000,
    });
  } catch (error) {
    console.error(`verify on ROM: ${error.message} (${path.relative(ROOT, dir)}/harness.log)`);
    return false;
  }
  const ew = fs.readFileSync(path.join(dir, 'ewram.bin')), iw = fs.readFileSync(path.join(dir, 'iwram.bin'));
  const rd = (addr, n) => { const b = addr >= 0x03000000 ? iw : ew, o = addr - (addr >= 0x03000000 ? 0x03000000 : 0x02000000); return n === 2 ? b.readInt16LE(o) : b.readUInt32LE(o); };
  const gs = rd(0x02000710, 4), scene = rd(gs + 0x5a4, 4);
  const players = s.types.globals.gPlayers.addr, p = rd(players, 4);
  const pos = p ? { x: rd(p + 0x30, 2), y: rd(p + 0x32, 2), z: rd(p + 0x34, 2) } : {};
  const rom = { scene, ...pos };
  const same = rom.scene === end.scene && rom.x === end.x && rom.z === end.z;
  console.log(`verify on ROM: ${same ? 'MATCH' : 'DIFFER'} rom ${JSON.stringify(rom)} port ${JSON.stringify(end)}`);
  if (args.includes('--coverage')) coverage(path.join(dir, 'total.exec'));
  for (const f of fs.readdirSync(dir)) if (/^total\./.test(f)) fs.rmSync(path.join(dir, f));
  return same;
}

// Per-function coverage of the ROM replay (harness trace: a byte per ROM halfword,
// bit 0 Thumb executed): executed bytes of each function in symbols/functions.csv.
function coverage(execPath) {
  const x = fs.readFileSync(execPath);
  const fns = fs.readFileSync(path.join(ROOT, 'symbols/functions.csv'), 'utf8').split('\n').slice(1)
    .map((l) => l.split(',')).filter((c) => /^[0-9A-F]{8}$/.test(c[0])).map((c) => ({ addr: parseInt(c[0], 16), name: c[1] }))
    .sort((a, b) => a.addr - b.addr);
  const out = {};
  let hit = 0;
  for (let i = 0; i < fns.length; i++) {
    const a = fns[i].addr - 0x08000000, e = (i + 1 < fns.length ? fns[i + 1].addr : fns[i].addr + 2) - 0x08000000;
    let n = 0;
    for (let o = a >> 1; o < e >> 1 && o < x.length; o++) if (x[o]) n++;
    if (n) { out[fns[i].name] = { addr: fns[i].addr.toString(16), bytes: 2 * n, size: e - a }; hit++; }
  }
  fs.writeFileSync(path.join(out_dir(), 'coverage.json'), JSON.stringify(out));
  console.log(`coverage: ${hit} of ${fns.length} functions executed on the ROM -> ${path.relative(ROOT, path.join(out_dir(), 'coverage.json'))}`);
}
function out_dir() { return out; }
