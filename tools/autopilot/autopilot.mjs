#!/usr/bin/env node
// The Autopilot: plays Boktai 3 on the source port from typed game state.
//
//   node tools/autopilot/autopilot.mjs m1 [--save F] [--out DIR] [--shots]
//   node tools/autopilot/autopilot.mjs story [--from MOVIE|--save SAV] [--until-scene SCENE|--until-story N] [--verify]
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
import { tickAt } from './movie-index.mjs';
import { distanceFromEntrance, routeDistances } from './route-entrances.mjs';
import { higherReturn } from './higher-landings.mjs';

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
const battery = opt('--save') || from?.save || null;
const s = await openSession({
  movie: from ? from.movie : undefined, solarLog: from ? from.solar : undefined,
  save: battery || undefined, render: shots, print: () => {},
  lib: [path.join(ROOT, 'port/agent/lib'), path.join(ROOT, 'tools/autopilot/lib')],
  onBoot: (g) => g.lib.message(),
});
const venvPython = path.join(ROOT, 'build/venv/bin/python');
const python = fs.existsSync(venvPython) ? venvPython : 'python3';
function stageData(file, generator, args, fallback) {
  file = path.join(ROOT, file);
  if (goal === 'story' && !fs.existsSync(file)) {
    const result = spawnSync(python, [path.join(ROOT, generator), ...args], { cwd: ROOT, encoding: 'utf8' });
    if (result.error || result.status !== 0) throw new Error(`Could not generate ${file}: ${result.error || result.stderr}`);
  }
  return fs.existsSync(file) ? JSON.parse(fs.readFileSync(file, 'utf8')) : fallback;
}
const STAGES = stageData('build/assets/stages.json', 'tools/assets/stages.py', ['json'], []);
const LOCKS = stageData('build/autopilot/locks.json', 'tools/autopilot/locks.py', [], {});
const PLATE_PUZZLES = stageData('build/autopilot/plate-puzzles.json', 'tools/autopilot/plate-puzzles.py', [], {});
const ESCORT_TARGETS = stageData('build/autopilot/escort-targets.json', 'tools/autopilot/escort-targets.py', [], {});
// What carries which reaction flag (tools/autopilot/breakers.py: Breakable_OnHit, the gun's
// lens table, weapon modifiers, read from the source and the ROM's weapon records).
const BREAKERS = (() => {
  const f = path.join(ROOT, 'build/autopilot/breakers.json');
  if (!fs.existsSync(f)) spawnSync(python, [path.join(ROOT, 'tools/autopilot/breakers.py'), f]);
  try { return JSON.parse(fs.readFileSync(f, 'utf8')); } catch { return { targets: { Breakable: {} }, lens: [], weapons: [] }; }
})();
// means(flag, bags): the lenses and swords that carry the flag, owned first
const means = (flag, bags) => [
  ...BREAKERS.lens.filter((l) => l.flags & flag).map((l) => ({ kind: 'lens', id: l.id, name: l.name, owned: bags.lenses.includes(l.id) })),
  ...BREAKERS.weapons.filter((w) => w.flags & flag).map((w) => ({ kind: 'sword', id: w.id, name: w.name, owned: bags.swords.includes(w.id) })),
].sort((a, b) => b.owned - a.owned);
const stageIndex = new Map(STAGES.map((stage) => [stage.scene, stage]));
const emptyStage = { items: [], doors: [], entrances: [] };
const stageOf = (scene) => stageIndex.get(scene) || emptyStage;
const log = [];
// scenes the run has been in (carried over from --from), for door choice
const UNREACHABLE = new Set(from && from.unreachable ? from.unreachable : []);  // 'scene:box' doors no route reached
const distancesTo = (scene, locked) => routeDistances(STAGES, scene, UNREACHABLE, locked);
const SEEN = new Map();  // person id -> rooms they were in
const VISITS = new Map();  // room visits this run: least visited doors first
const TRIED = new Set(from && from.tried ? from.tried : []);  // floor items already gone for
const VISITED = new Set(from ? [...(from.visited || []), ...(from.rooms || []).map((r) => r.scene)] : []);
let landingDetour;
const event = (what) => {
  const e = { frame: s.frame, ...what };
  log.push(e);
  console.log(JSON.stringify(e));
  if (e.skill === 'entry' || e.skill === 'room' || e.skill === 'goal')
    writeMovie('checkpoint.json', { scene: e.scene, x: e.x, y: e.y, z: e.z });
  if (e.skill === 'fight' && (e.dialogue !== undefined || e.trance || e.fight !== undefined))
    writeMovie('checkpoint.json', s.g.lib.where());
  if ((e.skill === 'purification' && e.phase !== undefined) || (e.skill === 'drive' && e.steps !== undefined))
    writeMovie('checkpoint.json', s.g.lib.where());
  if (e.skill === 'ride' && e.key)
    writeMovie('checkpoint.json', s.g.lib.where());
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
    if (!from) {
      if (battery) yield* L.continueSave();
      else if (!(yield* GOALS.m1(g, L))) return false;
    }
    const visited = VISITED, limit = +(opt('--scenes', 8));
    const destination = +opt('--until-scene', -1), routeGoal = +opt('--goal', destination);
    const storyGoal = +opt('--until-story', -1), endpoint = destination >= 0 || storyGoal >= 0;
    const reachedScene = () => destination >= 0 && L.where().scene === destination;
    const reached = () => reachedScene() || (storyGoal >= 0 && L.story() >= storyGoal);
    if (from?.landingDetour?.goal === routeGoal) landingDetour = from.landingDetour;
    let prev, keyGoal;
    const failedKeyRooms = new Set();
    const bossActive = () => L.fightTargets().some((target) => target.boss || target.barrier);
    for (let rooms = 0; rooms < limit; rooms++) {
      const doorsHere = (resolve = true) => {
        const doors = stageOf(L.where().scene).doors, hub = L.hub();
        if (hub) {
          const entries = doors.filter((d) => d.hub !== undefined && (!d.hubMenu || d.hubMenu === hub.kind));
          return resolve ? L.doorBranches(entries, (d) => d.hub) : entries;
        }
        const boxes = new Set(L.boxes().map((b) => b.name));
        const portals = doors.filter((d) => boxes.has(d.box));
        return resolve ? L.doorBranches(portals, (d) => d.box) : portals;
      };
      const ready = () => reached() || L.purificationPanel() || L.purificationBattle() || doorsHere(false).length > 0 || L.fightTargets().length > 0;
      if (!(yield* untilControl(g, L, ready, 120, 30000))) { event({ skill: 'stop', why: 'no control with a door or enemy in 30000 frames', ...L.where() }); return !endpoint && rooms > 0; }
      event({ skill: 'entry', ...L.where(), story: L.story() });
      if (reached()) {
        event({ skill: 'goal', reached: reachedScene() ? `scene ${destination}` : `story ${storyGoal}`, ...L.where(), story: L.story() });
        return true;
      }
      // Boss fights can advance the scene themselves. Rooms with exits can
      // defer patrols until a door is blocked or the player takes damage.
      const targets = L.fightTargets();
      if (targets.some((t) => t.boss || t.barrier) || (targets.length && !doorsHere(false).length)) {
        const f = yield* L.fight({ trace: (e) => event({ skill: 'fight', ...e }) });
        event({ skill: 'fought', ...f, scene: L.where().scene });
        if (!(yield* untilControl(g, L, ready, 60, 30000))) { event({ skill: 'stop', why: 'no control after the fight', ...L.where() }); return !endpoint && rooms > 0; }
        if (reached()) { rooms--; continue; }
      }
      const here = L.where().scene;
      if (landingDetour?.room === here && L.where().y >= landingDetour.height) {
        UNREACHABLE.delete(`${here}:${landingDetour.box}`);
        landingDetour = undefined;
      }
      if (L.purificationBattle()) {
        const result = yield* L.fightPurification({ trace: (e) => event({ skill: 'drive', ...e }) });
        event({ skill: 'drive-result', ...result, scene: L.where().scene, story: L.story() });
        if (result.done) continue;
        return false;
      }
      if (L.purificationPanel()) {
        const result = yield* L.preparePurification({ avoid: stageOf(here).doors.map((d) => d.box),
          trace: (e) => event({ skill: 'purification', ...e }) });
        event({ skill: 'purification-result', ...result, scene: L.where().scene, story: L.story() });
        if (result.done) continue;
        return false;
      }
      if (L.coffinAvailable()) {
        const before = L.story(), advanced = () => L.story() > before;
        for (const { box, cond } of ESCORT_TARGETS[here] || []) {
          if (L.conditionsAllow(cond) && L.boxes().some((b) => b.name === box) && (yield* throughDoor(g, L, box, { stopIf: advanced }))) break;
        }
        if (L.where().scene !== here || advanced()) continue;
      }
      // healing: below a third of the life bar with something in the item bag, find the
      // use of an item in the pause menu that raises life (menuSearch; once per room)
      {
        const p = $player(g) && g.$.gPlayers[0], life = () => g.$.gPlayers[0] ? g.$.gPlayers[0].currentStatA : 0;
        if (p && p.maximumStatA && p.currentStatA * 3 < p.maximumStatA && L.bags().items.length) {
          const life0 = p.currentStatA;
          // Judge the actual life gain so the search keeps solar fruit and
          // passes over items with unrelated effects.
          const r = yield* L.menuSearch(() => life() > life0,
            { max: 300, page: 1, keys: ['down', 'right', 'up', 'left', 'a', 'b'], settle: 90 });
          if (r.done) yield g.press(0, 120);
          event({ skill: 'heal', from: life0, to: life(), ...r });
        }
      }
      // Both HP traps and live pressure plates can run scripts on contact.
      let prioritizeExit = Boolean(landingDetour) || L.coffinAvailable() || L.movingFloors().length > 0;
      for (const puzzle of PLATE_PUZZLES[here] || []) {
        if (!L.conditionsAllow(puzzle.cond)) continue;
        const solved = yield* L.solvePlatePuzzle(puzzle, { avoid: stageOf(here).doors.map((d) => d.box),
          trace: (e) => event({ skill: 'nav', ...e }) });
        event({ skill: 'plate-puzzle', ...solved, scene: here });
        if (solved.done) {
          prioritizeExit = true;
          yield* untilControl(g, L, () => !g.fn.ScriptUtility_IsControlFlagSet(), 30, 1800);
        }
      }
      const switches = (stageOf(here).switches || []).concat(L.floorSwitches().filter((sw) => !sw.pressed));
      for (const sw of switches) {
        const key = `${here}:switch:${sw.box ?? sw.name}`;
        if (TRIED.has(key)) continue;
        const pressed = sw.addr ? () => L.floorSwitches().some((plate) => plate.addr === sw.addr && plate.pressed) : undefined;
        const opts = { avoid: stageOf(here).doors.map((d) => d.box) };
        const probe = yield* L.probeFloorSwitch(sw, opts);
        if (probe?.hazard) {
          prioritizeExit = true;
          event({ skill: 'switch', name: sw.name, reached: false, reason: 'falling object at the switch' });
          continue;
        }
        const delayed = probe?.ready;
        const r = delayed ? { reason: 'released before delayed script' } :
          probe?.pressed ? { reason: 'pressed' } :
          yield* L.navigate([sw.x, sw.z], { ...opts, stopIf: pressed, trace: (e) => event({ skill: 'nav', ...e }) });
        const clear = () => !g.fn.ScriptUtility_IsControlFlagSet();
        const completed = delayed ? (yield* untilControl(g, L, () => delayed() && clear(), 24, 900)) :
          pressed ? pressed() || L.where().scene !== here : r.reached;
        prioritizeExit ||= Boolean(delayed && completed);
        if (!pressed || completed) TRIED.add(key);
        if (pressed && completed && !delayed) yield* untilControl(g, L, clear, 24, 900);
        else yield* L.clearInterruptions({});
        event({ skill: 'switch', box: sw.box, name: sw.name, reached: completed, reason: r.reason });
        if (L.where().scene !== here) break;
      }
      if (L.where().scene !== here) { rooms--; continue; }
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
          const frame = yield* L.equipBestFrame();
          if (frame) event({ skill: 'equip', ...frame });
          const ch = yield* L.chargeEnergy(6);
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
      // Cross a scripted opening before it resets. Defer detours when a plate
      // preview has shown an attack on the route to an optional reward.
      if (!prioritizeExit && !opt('--no-talk')) {
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
          if (reached()) break;
          const t = t0;
          const story0 = L.story(), now = t.addr ? L.people().find((q) => q.addr === t.addr) : null;
          const r = yield* L.interact(now ? now.x : t.x, now ? now.z : t.z, { avoid: stageOf(here).doors.map((d) => d.box), trace: (e) => event({ skill: 'nav', ...e }) });
          if (L.shopState()) {
            const purchase = yield* L.buyHealing();
            event({ skill: 'shop', scene: here, ...purchase });
            yield* L.leaveShop();
          }
          // a shop or service menu the talk opened: back out of it
          for (let i = 0; i < 8 && !$player(g); i++) { yield g.press('b', 2); yield g.press(0, 10); yield* L.clearInterruptions({}); }
          if (round === 0 && r.done) TRIED.add(tkey(t));  // a failed talk is tried again on the next visit
          event({ skill: 'talk', id: t.key, round, x: t.x, z: t.z, done: r.done, reason: r.reason, lines: (r.lines || []).slice(0, 6), story: L.story(), was: story0, facilities: L.facilities() });
          // whatever the talk started (a choice, a menu, an event) ends before the next one
          yield* untilControl(g, L, () => true, 30, 1200);
          if (L.where().scene !== here || !r.done || companion(t.key)) break;
        }
      }
      if (reached()) { rooms--; continue; }
      // A key detour has a specific pickup. Other items can wait until the
      // detour ends, since the approach can consume life or change floors.
      if (keyGoal && L.hasItem(keyGoal.item)) keyGoal = undefined;
      const pickups = prioritizeExit ? [] : stageOf(here).items.filter((it) => !keyGoal || (it.category === 0 && it.item === keyGoal.item));
      for (const it of pickups.filter((it) => !TRIED.has(`${here}:${it.x},${it.z}`))) {
        if (bossActive()) break;
        TRIED.add(`${here}:${it.x},${it.z}`);
        const life = L.where().life;
        const tooCostly = () => L.where().life < life - Math.max(4, Math.floor(life / 10));
        const c = yield* L.collect(it.x, it.z, { avoid: stageOf(here).doors.map((d) => d.box), stopIf: () => bossActive() || tooCostly(),
          trace: (e) => event({ skill: 'nav', ...e }) });
        event({ skill: 'collect', item: it.name || `${it.category}:${it.item}`, x: it.x, z: it.z, ...c });
        if (c.reason === 'no path' || (c.reason === 'stopped' && !tooCostly())) TRIED.delete(`${here}:${it.x},${it.z}`);
        if (L.where().scene !== here || c.reason === 'stopped' || tooCostly()) break;
      }
      if (reached() || L.where().scene !== here) { rooms--; continue; }
      visited.add(here);
      if (prev !== undefined) visited.add(prev);
      if (keyGoal && L.hasItem(keyGoal.item)) keyGoal = undefined;
      if (keyGoal && here === keyGoal.scene) {
        failedKeyRooms.add(`${keyGoal.item}:${here}`);
        keyGoal = undefined;
      }
      // --toward SCENE: a door there first; --goal SCENE: the door on the shortest route
      // over the door graph (stages.json); then unvisited scenes
      const toward = rooms === 0 ? +opt('--toward', -1) : -1;
      // with no --goal: explore, the door on the shortest route (over doors not known to
      // be unreachable) to the nearest scene not visited yet
      const dist = keyGoal ? distancesTo(keyGoal.scene, keyGoal.blocked)
        : routeGoal >= 0 ? distancesTo(landingDetour?.via ?? routeGoal) : exploreDistances(here, visited);
      VISITS.set(here, (VISITS.get(here) || 0) + 1);
      const distanceFromDoor = (d) => distanceFromEntrance(d, stageOf, dist);
      const returnDoor = (d) => landingDetour?.via === here && d.box === landingDetour.returnBox && d.entrance === landingDetour.entrance;
      const doors = doorsHere().sort((a, b) => returnDoor(b) - returnDoor(a) || (b.scene === toward) - (a.scene === toward) ||
        (dist.get(a.scene) ?? 1e9) - (dist.get(b.scene) ?? 1e9) || visited.has(a.scene) - visited.has(b.scene) ||
        (VISITS.get(a.scene) || 0) - (VISITS.get(b.scene) || 0) || distanceFromDoor(a) - distanceFromDoor(b));
      const lock = doors.length && L.closedLock(LOCKS[here] || [], doors[0].box);
      if (!keyGoal && lock && !L.hasItem(lock.item)) {
        const blocked = `${here}:${lock.box}`;
        const keyRoutes = STAGES.filter((st) => st.scene !== here && !failedKeyRooms.has(`${lock.item}:${st.scene}`) &&
            st.items.some((it) => it.category === 0 && it.item === lock.item))
          .map((st) => ({ scene: st.scene, dist: distancesTo(st.scene, blocked) }))
          .filter((r) => r.dist.has(here)).sort((a, b) => a.dist.get(here) - b.dist.get(here));
        if (keyRoutes.length) {
          const route = keyRoutes[0];
          keyGoal = { item: lock.item, scene: route.scene, blocked };
          doors.sort((a, b) => (a.box === lock.box) - (b.box === lock.box) ||
            (route.dist.get(a.scene) ?? Infinity) - (route.dist.get(b.scene) ?? Infinity) ||
            distanceFromEntrance(a, stageOf, route.dist) - distanceFromEntrance(b, stageOf, route.dist));
          event({ skill: 'key', scene: here, box: lock.box, item: lock.item, toward: route.scene });
        }
      }
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
          const edge = `${here}:${d.box}`;
          const floor = L.where().y;
          const alternate = !keyGoal && !landingDetour && routeGoal >= 0 && d.y > floor &&
            higherReturn(STAGES, stageOf(here), d, floor, (scene) => distancesTo(scene, edge));
          if (yield* throughDoor(g, L, d.box, { alternateLanding: Boolean(alternate) })) { UNREACHABLE.delete(edge); moved = true; break; }
          if (L.where().scene === here) UNREACHABLE.add(edge);
          if (L.where().scene !== here) { moved = true; break; }
          if (alternate) {
            landingDetour = { ...alternate, goal: routeGoal };
            event({ skill: 'landing', ...alternate });
          }
        }
      }
      if (!moved) { event({ skill: 'stop', why: 'no door worked', ...L.where() }); return !endpoint && rooms > 0; }
    }
    return !endpoint || reached();
  },

  // With --from: nothing more (to verify, cover and save states for an earlier run).
  *replay(g) { yield 1; return true; },

  // With --from: hold --keys (default none) for --frames frames, optionally
  // changing the raw solar threshold with --lux, and log live state.
  *hold(g, L) {
    const keys = opt('--keys', '') || 0, n = +opt('--frames', 3000);
    if (opt('--lux') !== undefined) s.setSolar(+opt('--lux'));
    for (let i = 0; i < n; i += 300) {
      yield g.press(keys, Math.min(300, n - i));
      event({ skill: 'hold', keys, ...L.where(), story: L.story(), solar: L.solarStatus() });
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

// Through door box BOX: navigate; when a scripted gate turns the player back, open
// the room's chests and items (the usual ask), then try again.
function* throughDoor(g, L, box, { stopIf, alternateLanding } = {}) {
  const scene = L.where().scene, tried = new Set();
  const avoid = stageOf(scene).doors.map((d) => d.box).filter((b) => b !== box);
  const landing = yield* L.approachLift(box, { avoid, trace: (e) => event({ skill: 'nav', ...e }) });
  if (landing) event({ skill: 'lift', box, ...landing });
  if (L.where().scene !== scene) return true;
  if (L.coffinAvailable()) {
    let foughtPatrol = false;
    for (let attempt = 0; attempt < 3 && L.coffinAvailable(); attempt++) {
      yield* untilControl(g, L, () => !g.fn.ScriptUtility_IsControlFlagSet(), 30, 2400);
      if (L.where().scene !== scene) return true;
      if (!L.coffinAvailable() || !g.$.gPlayers[0]) break;
      const life = L.where().life;
      const r = yield* L.carryCoffin(box, { avoid, stopIf, trace: (e) => event({ skill: 'nav', ...e }) });
      event({ skill: 'coffin', box, ...L.where(), ...r });
      if (r.reached || L.where().scene !== scene || stopIf?.()) return true;
      if (r.reason !== 'lost grip') break;
      if (!foughtPatrol && L.where().life < life && L.fightTargets().length) {
        foughtPatrol = true;
        const f = yield* L.fight({ frames: 600, trace: (e) => event({ skill: 'fight', ...e }) });
        event({ skill: 'fought', ...f, scene: L.where().scene });
        if (L.where().scene !== scene) return true;
      }
    }
    if (L.coffinAvailable()) return false;
  }
  const lock = L.closedLock(LOCKS[scene] || [], box);
  if (lock && L.hasItem(lock.item)) {
    const r = yield* L.openKeyDoor(lock, { avoid, trace: (e) => event({ skill: 'nav', ...e }) });
    event({ skill: 'unlock', scene, box, item: lock.item, ...r });
    if (L.where().scene !== scene) return true;
  }
  let fights = 0;
  for (let attempt = 0; attempt < 6; attempt++) {
    const floor = yield* L.approachMovingFloor(box, { avoid });
    if (floor?.name !== undefined) {
      const { gaps, ...trip } = floor;
      event({ skill: 'moving-floor', box, ...trip });
    }
    const initial = new Set(L.fightTargets().map((t) => t.addr));
    const life = () => g.$.gPlayers[0]?.currentStatA ?? 0;
    const lifeBefore = life();
    const lossBudget = Math.max(4, Math.floor(lifeBefore / 10));
    const interrupted = () => {
      const targets = L.fightTargets();
      return targets.some((t) => t.boss || t.barrier || !initial.has(t.addr)) ||
        (targets.length && lifeBefore - life() >= lossBudget);
    };
    const r = yield* L.navigate(box, { avoid, avoidCells: floor?.gaps, stopIf: interrupted, trace: (e) => event({ skill: 'nav', ...e }) });
    event({ skill: 'navigate', box, ...r, from: scene });
    if (r.reached) return true;
    if (alternateLanding && r.reason === 'no path') return false;
    if (L.fightTargets().length && fights < 5 && (!lock || L.hasItem(lock.item))) {
      fights++;
      const f = yield* L.fight({ trace: (e) => event({ skill: 'fight', ...e }) });
      event({ skill: 'fought', ...f, scene: L.where().scene });
      if (L.where().scene !== scene) return true;
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

// End a run after 30 still frames (at most 600), including elevation and scene
// changes so the final comparison is outside a fall or entrance animation.
function* settle(g) {
  let last = '', still = 0;
  for (let i = 0; i < 600 && still < 30; i++) {
    yield 1;
    const p = g.$.gPlayers[0];
    const k = p ? `${g.lib.where().scene}:${p.position.x},${p.position.y},${p.position.z}` : '-';
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
    const field = $player(g) || (L.hub() && L.hub().state === 'Registry3F74_UpdateSelection');
    calm = field && ready() ? calm + 1 : 0;
    if (calm >= quiet) return true;
    // an unknown menu holds the screen (no player, no message, no known menu): back out
    // with B, or START (the bike parts menu closes on START when it was opened from a
    // story scene: ModifierMenu_HandleOverviewInput), now and then confirm with A
    stuck = field ? 0 : stuck + 1;
    const bike = !field && L.bikeRider();
    if (bike && !msg.open) {
      const result = yield* L.rideBike(bike);
      if (result) event({ skill: 'ride', ...L.where(), ...result });
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
let readFrames = from?.readFrames?.slice() ?? [];
let rooms = from?.rooms?.slice() ?? [];
const snapshot = s.snapshot, restore = s.restore;
s.snapshot = () => ({ ...snapshot(), readFrames: readFrames.slice(), rooms: rooms.slice() });
s.restore = (snap) => { restore(snap); readFrames = snap.readFrames.slice(); rooms = snap.rooms.slice(); };
const { ok, end } = s.run(function* (g) {
  g.lib.session = s;
  const gameState = g.types.globals.gGameState.addr;
  g.on('Input_UpdatePadStates', () => {
    if (readFrames.at(-1) !== s.frame) readFrames.push(s.frame);
    const state = g.u32(gameState), scene = state ? g.u32(state + 0x5a4) : 0;
    if (scene && rooms.at(-1)?.scene !== scene) rooms.push({ frame: s.frame, scene });
  });
  // --life N: set the player's life to N before the goal (a port-only test aid, e.g. of
  // healing; such a run cannot be verified on the ROM)
  if (opt('--life') && g.$.gPlayers[0]) g.$.gPlayers[0].currentStatA = +opt('--life');
  const reached = yield* GOALS[goal](g, g.lib);
  yield* settle(g);
  const w = g.lib.where();
  return { ok: reached, end: { scene: w.scene, story: g.lib.story(), x: w.x, y: w.y, z: w.z,
    ...(s.solarLog.length ? { solar: g.lib.solarStatus() } : {}) } };
});
const secs = (performance.now() - t0) / 1000;

const movie = s.movie.map((k) => k | 0);
readFrames = readFrames.filter((n) => n < movie.length);
rooms = rooms.filter((entry) => entry.frame < movie.length);
const reads = readFrames.map((n) => movie[n]);
const saves = rooms.map(({ frame, scene }) => ({ at: tickAt(readFrames, frame), name: `scene_${scene}_f${frame}` }));
writeMovie('movie.json', end);
fs.writeFileSync(path.join(out, 'harness.txt'), harnessScript(reads, saves, `autopilot ${goal}: ${ok ? 'reached' : 'NOT reached'}; port end ${JSON.stringify(end)}`));
fs.writeFileSync(path.join(out, 'log.json'), JSON.stringify(log, null, 1));
console.log(`${goal}: ${ok ? 'reached' : 'NOT reached'} at frame ${s.frame} (${secs.toFixed(1)} s), end ${JSON.stringify(end)} -> ${path.relative(ROOT, out)}`);
let verified = true;
if (args.includes('--verify') && opt('--life')) console.log('--verify skipped: --life changed the port\'s memory');
else if (args.includes('--verify')) verified = verify(end);
process.exit(ok && verified ? 0 : 1);

// Refresh the resume point at each room before trying its exits. If the
// port traps while testing a later step, this still contains accepted input.
function writeMovie(file, end) {
  const frames = s.movie.length;
  const movie = {
    goal, save: battery, frames, end, visited: [...VISITED], tried: [...TRIED], unreachable: [...UNREACHABLE],
    landingDetour,
    rooms: rooms.filter((entry) => entry.frame < frames), solar: s.solarLog,
    readFrames: readFrames.slice(0, tickAt(readFrames, frames)), movie: s.movie,
  };
  const dst = path.join(out, file);
  fs.writeFileSync(dst + '.tmp', JSON.stringify(movie));
  fs.renameSync(dst + '.tmp', dst);
}

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
  const lux = s.solarLog.map(([f, v]) => ({ at: tickAt(readFrames, f), lux: v }));
  const marks = [...saves, ...lux].sort((a, b) => a.at - b.at);
  let m = 0;
  for (let i = 0; i < reads.length;) {
    while (m < marks.length && marks[m].at <= i) { const k = marks[m++]; lines.push(k.lux !== undefined ? `lux ${k.lux}` : `save ${k.name}`); }
    let j = i;
    const stop = m < marks.length ? marks[m].at : Infinity;
    while (j < reads.length && reads[j] === reads[i] && j < stop) j++;
    const keys = NAMES.filter((_, b) => reads[i] & (1 << b)).join('+') || 'NONE';
    lines.push(`holdt ${keys} ${j - i}`);
    i = j;
  }
  for (; m < marks.length; m++) { const k = marks[m]; lines.push(k.lux !== undefined ? `lux ${k.lux}` : `save ${k.name}`); }
  lines.push('holdt NONE 1', 'dump ewram 02000000:40000', 'dump iwram 03000000:8000', 'shot end');
  return lines.join('\n') + '\n';
}

// Replay harness.txt on baserom.gba and compare the scene and player position
// (gGameState->unk5A4 and the first player's position) with the port's.
function verify(end) {
  const dir = path.join(out, 'rom');
  fs.mkdirSync(dir, { recursive: true });
  const harness = path.join(ROOT, 'tools/emu/harness');
  const env = { ...process.env };
  delete env.BOKTAI3_SAV;
  delete env.BOKTAI3_KEY_TICK_PC;
  if (battery) env.BOKTAI3_SAV = path.resolve(battery);
  // Input_UpdatePadStates starts with PUSH and two literal loads. Its first
  // LDRH reads REG_KEYINPUT once before any branches, which permits untraced
  // replays to use mGBA's normal frame loop while keeping game-tick input.
  const input = s.g.addr('Input_UpdatePadStates') & ~1;
  if (s.g.u16(input + 6) === 0x8802) env.BOKTAI3_KEY_TICK_PC = (input + 6).toString(16);
  const r = spawnSync(harness, [path.join(ROOT, 'baserom.gba'), path.join(out, 'harness.txt'), dir], { env, encoding: 'utf8', maxBuffer: 1 << 26 });
  if (r.error || r.status !== 0) throw new Error(`ROM harness failed: ${r.error || r.stderr || r.signal || r.status}`);
  const warn = (r.stderr || '').split('\n').filter((l) => /ticks$|not reached|times in|error/i.test(l));
  const ew = fs.readFileSync(path.join(dir, 'ewram.bin')), iw = fs.readFileSync(path.join(dir, 'iwram.bin'));
  const rd = (addr, n) => { const b = addr >= 0x03000000 ? iw : ew, o = addr - (addr >= 0x03000000 ? 0x03000000 : 0x02000000); return n === 2 ? b.readInt16LE(o) : b.readUInt32LE(o); };
  const gs = rd(0x02000710, 4), scene = gs ? rd(gs + 0x5a4, 4) : undefined;
  const players = s.types.globals.gPlayers.addr, p = rd(players, 4);
  const pos = p ? { x: rd(p + 0x30, 2), y: rd(p + 0x32, 2), z: rd(p + 0x34, 2) } : {};
  const vars = rd(s.types.globals.gGameVariables.addr, 4);
  const rom = { scene, story: vars ? rd(vars + 0x3c, 2) : null, ...pos };
  if (end.solar) rom.solar = { measurement: rd(s.types.globals.gSolarMeasurement.addr, 4) | 0,
    intensity: gs ? rd(gs + 0x874, 2) : 0, level: gs ? rd(gs + 0x876, 2) : 0 };
  const same = !warn.some((line) => line.startsWith('holdt:')) &&
    rom.scene === end.scene && rom.story === end.story &&
    rom.x === end.x && rom.y === end.y && rom.z === end.z &&
    (!end.solar || (rom.solar.intensity === end.solar.intensity && rom.solar.level === end.solar.level));
  console.log(`verify on ROM: ${same ? 'MATCH' : 'DIFFER'} rom ${JSON.stringify(rom)} port ${JSON.stringify(end)}${warn.length ? ' ' + warn.join('; ') : ''}`);
  if (same && args.includes('--coverage')) coverage(path.join(dir, 'total.exec'));
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
