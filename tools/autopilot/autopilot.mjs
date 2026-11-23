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
  movie: from ? from.movie : undefined,
  save: opt('--save') || (from && from.save) || undefined, render: shots, print: () => {},
  lib: [path.join(ROOT, 'port/agent/lib'), path.join(ROOT, 'tools/autopilot/lib')],
});
// Script-derived stage data (doors, entrances, items, enemies): tools/assets/stages.py json.
const STAGES = (() => { try { return JSON.parse(fs.readFileSync(path.join(ROOT, 'build/assets/stages.json'), 'utf8')); } catch { return []; } })();
const stageOf = (scene) => STAGES.find((x) => x.scene === scene) || { items: [], doors: [], entrances: [] };
const log = [];
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

// Through door box BOX: navigate; when a scripted gate turns the player back, open
// the room's chests and items (the usual ask), then try again.
function* throughDoor(g, L, box) {
  const scene = L.where().scene, tried = new Set();
  for (let attempt = 0; attempt < 6; attempt++) {
    const r = yield* L.navigate(box, { trace: (e) => event({ skill: 'nav', ...e }) });
    event({ skill: 'navigate', box, ...r, from: scene });
    if (r.reached) return true;
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
// open for `quiet` frames (and `ready()` holds, e.g. the room has a given box).
function* untilControl(g, L, ready = () => true, quiet = 120, limit = 60000) {
  const msg = L.message();
  for (let calm = 0; g.frameNo < limit;) {
    const step = yield* L.menuStep({});
    if (step) { event({ skill: 'menu', ...step }); calm = 0; continue; }
    if (msg.open) { yield* L.advanceText(600); event({ skill: 'dialogue', string: msg.id, scene: L.where().scene }); calm = 0; continue; }
    yield 1;
    const field = $player(g);
    calm = field && ready() ? calm + 1 : 0;
    if (calm >= quiet) return true;
  }
  return false;
}
const $player = (g) => g.$.gPlayers[0] && g.lib.tasks().some((t) => !t.dead && t.update === 'Player_Update');

if (!GOALS[goal]) { console.error(`unknown goal ${goal} (${Object.keys(GOALS).join(' ')})`); process.exit(2); }
const t0 = performance.now();
// The keys each input read saw (Input_UpdatePadStates runs once per game tick; the
// port has no lag frames, the hardware does, so replays key inputs to these reads).
const reads = from && from.reads ? from.reads.slice() : [];
const ok = s.run(function* (g) {
  g.on('Input_UpdatePadStates', () => { reads.push(s.movie[s.frame] | 0); });
  const reached = yield* GOALS[goal](g, g.lib);
  yield* settle(g);
  return reached;
});
const secs = (performance.now() - t0) / 1000;
const end = s.run(function* (g) { const w = g.lib.where(); return { scene: w.scene, x: w.x, y: w.y, z: w.z }; });

const movie = s.movie.map((k) => k | 0);
fs.writeFileSync(path.join(out, 'movie.json'), JSON.stringify({ goal, save: opt('--save') || (from && from.save) || null, frames: movie.length, end, reads, movie }));
fs.writeFileSync(path.join(out, 'harness.txt'), harnessScript(reads, s.exports, `autopilot ${goal}: ${ok ? 'reached' : 'NOT reached'}; port end ${JSON.stringify(end)}`));
fs.writeFileSync(path.join(out, 'log.json'), JSON.stringify(log, null, 1));
console.log(`${goal}: ${ok ? 'reached' : 'NOT reached'} at frame ${s.frame} (${secs.toFixed(1)} s), end ${JSON.stringify(end)} -> ${path.relative(ROOT, out)}`);
let verified = true;
if (args.includes('--verify')) verified = verify(end);
process.exit(ok && verified ? 0 : 1);

// The run as a tools/emu/harness script for the original ROM: `holdt KEYS N` holds
// keys for N calls of Input_UpdatePadStates (tick), so the ROM sees the same keys
// at the same game ticks however many lag frames it has. Ends with a dump of EWRAM
// (verify reads the scene and player position from it) and a screenshot. Needs the
// harness with tick/holdt (tools/emu/harness.c; `make -C tools/emu harness`).
function harnessScript(reads, ex, title) {
  const NAMES = ['A', 'B', 'SELECT', 'START', 'RIGHT', 'LEFT', 'UP', 'DOWN', 'R', 'L'];
  const tick = s.g.addr('Input_UpdatePadStates') & ~1;
  const time = Math.floor(Date.parse('2025-11-13T12:00:00Z') / 1000);
  const lines = [`# ${title}`, '# generated by tools/autopilot/autopilot.mjs (inputs keyed to game ticks)',
    'trace off', `rtc ${time}`, 'lux 232', `tick ${tick.toString(16).toUpperCase()}`];
  for (let i = 0; i < reads.length;) {
    let j = i;
    while (j < reads.length && reads[j] === reads[i]) j++;
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
  const harness = path.join(ROOT, 'tools/emu/harness');
  const r = spawnSync(harness, [path.join(ROOT, 'baserom.gba'), path.join(out, 'harness.txt'), dir], { encoding: 'utf8', maxBuffer: 1 << 26 });
  const warn = (r.stderr || '').split('\n').filter((l) => /ticks$|not reached|times in|error/i.test(l));
  const ew = fs.readFileSync(path.join(dir, 'ewram.bin')), iw = fs.readFileSync(path.join(dir, 'iwram.bin'));
  const rd = (addr, n) => { const b = addr >= 0x03000000 ? iw : ew, o = addr - (addr >= 0x03000000 ? 0x03000000 : 0x02000000); return n === 2 ? b.readInt16LE(o) : b.readUInt32LE(o); };
  const gs = rd(0x02000710, 4), scene = rd(gs + 0x5a4, 4);
  const players = s.types.globals.gPlayers.addr, p = rd(players, 4);
  const pos = p ? { x: rd(p + 0x30, 2), y: rd(p + 0x32, 2), z: rd(p + 0x34, 2) } : {};
  const rom = { scene, ...pos };
  const same = rom.scene === end.scene && rom.x === end.x && rom.z === end.z;
  console.log(`verify on ROM: ${same ? 'MATCH' : 'DIFFER'} rom ${JSON.stringify(rom)} port ${JSON.stringify(end)}${warn.length ? ' ' + warn.join('; ') : ''}`);
  for (const f of fs.readdirSync(dir)) if (/^total\./.test(f)) fs.rmSync(path.join(dir, f));
  return same;
}
