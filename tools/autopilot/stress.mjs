#!/usr/bin/env node
// Exercise pause menus and field controls from a checkpoint, then compare the
// port and ROM at the same input ticks. Keep the movie when a check fails.
import fs from 'node:fs';
import path from 'node:path';
import { parseArgs } from 'node:util';
import { setFlagsFromString } from 'node:v8';
import { openSession } from '../../port/agent/session.mjs';
import { root, rom, clock, lux, holdTicks, runRom } from './lib/rom.mjs';
import { replayAgainstRom } from './lib/rom-reference.mjs';

// A wasm fault must reach the catch below so we can replay its input prefix.
// On this host Node's signal-based bounds checks can exit with SIGBUS instead.
setFlagsFromString('--wasm-enforce-bounds-checks');

const { values: opts } = parseArgs({ options: {
  save: { type: 'string' }, out: { type: 'string' }, scene: { type: 'string' },
  frames: { type: 'string', default: '2800' }, seed: { type: 'string', default: '1' },
  mode: { type: 'string', default: 'fuzz' }, from: { type: 'string' }, wasm: { type: 'string' },
  'rom-reference': { type: 'string' },
}});

if (!opts.save && !opts.from) throw new Error('Supply --save FILE or --from DIR/movie.json');
if (!['fuzz', 'smoke', 'swords', 'reset'].includes(opts.mode)) throw new Error('Modes are fuzz, smoke, swords and reset');
const replay = opts.from ? JSON.parse(fs.readFileSync(opts.from, 'utf8')) : null;
const mode = replay?.mode ?? opts.mode;
const save = opts.save || replay?.save;
const replayKeys = replay?.movie ?? replay?.keyRuns?.flatMap(([keys, count]) => Array(count).fill(keys));
if (replay && !replayKeys?.length) throw new Error('Replay contains no controller input');
const frames = Number(opts.frames);
let seed = Number(opts.seed);
if (!Number.isSafeInteger(frames) || frames < 1 || frames > 20000 ||
    !Number.isInteger(seed) || seed < 1 || seed > 0xffffffff) throw new Error('Invalid frame count or seed');
const out = path.resolve(opts.out || path.join(root, 'build/autopilot/stress', path.basename(save || 'power-on', '.sav')));
const romOut = path.join(out, 'rom');
const reference = opts['rom-reference'] && path.resolve(opts['rom-reference'], 'rom');
if (reference && (reference === romOut ||
    (fs.existsSync(reference) && fs.existsSync(romOut) && fs.realpathSync(reference) === fs.realpathSync(romOut)))) {
  throw new Error('Use a separate output directory when comparing with a ROM reference');
}
fs.mkdirSync(romOut, { recursive: true });
const inputs = path.join(out, 'inputs.jsonl');
fs.writeFileSync(inputs, '');

const readFrames = [], reads = [], samples = [];
const boots = [], equipmentDetails = [];
const missingReturns = new Set();
const auditPath = path.join(root, 'build/port/game/missing_returns.txt');
const auditNames = mode === 'fuzz' && fs.existsSync(auditPath)
  ? [...fs.readFileSync(auditPath, 'utf8').matchAll(/^src\/[^:]+: (\w+) \(/gm)].map((m) => m[1])
  : [];
let firstCheck = replay?.firstCheck ?? 0;
let fault = null;
let faultAt = null;
let tick, regions;
const s = await openSession({
  save, lib: ['port/agent/lib', 'tools/autopilot/lib'].map((p) => path.join(root, p)),
  wasm: opts.wasm, movie: replayKeys, solar: lux, time: clock, render: mode === 'swords',
  onBoot(g, session) {
    tick = g.addr('Input_UpdatePadStates') & ~1;
    regions = sampleRegions(g.types);
    g.on('AgbMain', () => boots.push(session.frame));
    if (mode === 'swords')
      g.on('SceneHud_ShowEquipmentDetails', () => equipmentDetails.push(session.frame));
    for (const name of new Set(auditNames)) {
      if (g.types.functions[name]) g.on(name, () => missingReturns.add(name), { once: true });
    }
    g.on('Input_UpdatePadStates', () => {
      readFrames.push(session.frame);
      reads.push(session.movie[session.frame] ?? 0);
      samples.push(sample(session.memory.buffer, regions));
    });
  },
});

try {
  s.run(function* (g) {
    if (replay) {
      if (replay.scene !== undefined && samples[firstCheck]?.readUInt32LE(0) !== replay.scene)
        throw new Error(`Expected replay checkpoint scene ${replay.scene} at tick ${firstCheck}`);
      for (const [name, count] of Object.entries({ ticks: reads.length, boots: boots.length,
        equipmentDetails: equipmentDetails.length })) {
        if (typeof replay[name] === 'number' && count !== replay[name])
          throw new Error(`Expected replay ${name}=${replay[name]}, observed ${count}`);
      }
      return;
    }
    const press = g.press;
    g.press = (keys, frames) => {
      // Record the requested input before running it so a hang also leaves
      // the action and its starting frame.
      fs.appendFileSync(inputs, JSON.stringify({ frame: g.frameNo, keys, frames }) + '\n');
      return press(keys, frames);
    };
    const start = yield* g.lib.continueSave();
    firstCheck = reads.length;
    console.log(JSON.stringify({ start, firstCheck }));
    if (opts.scene !== undefined && start.scene !== Number(opts.scene))
      throw new Error(`Checkpoint loaded scene ${start.scene}, expected ${opts.scene}`);
    if (mode === 'reset') {
      yield g.press('a+b+select+start', 2);
      const continued = yield* g.lib.continueSave();
      if (boots.length !== 2) throw new Error(`Expected one reboot, saw ${boots.length - 1}`);
      if (continued.scene !== start.scene || continued.life !== start.life)
        throw new Error(`Continue after reset changed scene or life: ${JSON.stringify(continued)}`);
    } else if (mode === 'smoke') {
      // The same short movie for every area. Exercise the field, then try
      // opening, switching and dismissing a pause page with ordinary input.
      for (const [key, wait] of [['right', 18], ['b', 20], ['start', 40], ['r', 28], ['b', 35]])
        yield* tap(g, key, wait);
    } else {
      const reached = yield* swords(g);
      if (mode === 'swords' && !reached) throw new Error('Did not reach the swords page');
      if (mode === 'fuzz') yield* fuzz(g);
    }
    // Include an input read after the last action so its effects are compared.
    yield g.press(0, 2);
  });
} catch (error) {
  fault = String(error?.stack || error);
  faultAt = { tick: reads.length, frame: s.frame };
}
const movie = replayKeys || s.movie;
const run = { save: save ? path.relative(root, path.resolve(save)) : null, movie, readFrames, firstCheck, seed: Number(opts.seed),
  mode, boots, fault };
fs.writeFileSync(path.join(out, 'movie.json'), JSON.stringify(run));
const script = path.join(out, 'harness.txt');
const spec = regions.map(({ address, offset, size }) =>
  `${address.toString(16)}${offset === undefined ? '' : '+' + offset.toString(16)}:${size.toString(16)}`).join(',');
const lines = ['trace off', `rtc ${Date.parse(clock) / 1000}`, `lux ${lux}`, `fasttick ${tick.toString(16)}`,
  `tickdump ticks 0:${Math.max(1, reads.length)} ${spec}`, ...holdTicks(reads)];
fs.writeFileSync(script, lines.join('\n') + '\n');
fs.writeFileSync(path.join(out, 'port.bin'), Buffer.concat(samples));

const romSamples = path.join(romOut, 'ticks.bin');
let difference;
try {
  const ticks = replayAgainstRom({
    rom, save, script, out: romOut, reference,
    expectedBytes: reads.length * regions.reduce((length, region) => length + region.size, 0),
    run: () => runRom(script, romOut, { save, render: false, timeout: 480_000 }),
  });
  difference = compare(ticks, samples, regions, readFrames);
} catch (error) {
  const partial = fs.existsSync(romSamples) && compare(fs.readFileSync(romSamples), samples, regions, readFrames);
  difference = { ...partial, error: String(error) };
}
const result = { save: run.save, seed: run.seed, ticks: reads.length, frames: movie.length, firstCheck, boots,
  equipmentDetails, missingReturns: [...missingReturns].sort(), difference, fault, faultAt };
fs.writeFileSync(path.join(out, 'result.json'), JSON.stringify(result, null, 2) + '\n');
console.log(`${difference || fault ? 'DIFFER' : 'MATCH'} ${JSON.stringify(result)}`);
process.exitCode = difference || fault ? 1 : 0;

function sampleRegions(types) {
  const addr = (name) => {
    const a = types.globals[name]?.addr;
    if (!a) throw new Error(`Missing typed global ${name}. Run port/agent/gametypes.py`);
    return a;
  };
  const member = (type, name) => {
    const f = types.types[`struct ${type}`]?.fields.find((x) => x.name === name);
    if (!f) throw new Error(`Missing ${type}.${name} in the game type map`);
    return f.off;
  };
  return [
    { name: 'scene', address: addr('gGameState'), offset: member('GameState', 'unk5A4'), size: 4 },
    { name: 'position', address: addr('gPlayers'), offset: member('Player', 'position'), size: 6 },
    { name: 'life', address: addr('gPlayers'), offset: member('Player', 'currentStatA'), size: 2 },
    { name: 'energy', address: addr('gPlayers'), offset: member('Player', 'currentStatB'), size: 2 },
    { name: 'hudCallbacks', address: addr('gFieldHud'), offset: member('SceneHudState', 'updateCallback'), size: 8 },
    { name: 'hudPage', address: addr('gFieldHud'), offset: member('SceneHudState', 'mode'), size: 1 },
    { name: 'previousInput', address: addr('gPadState'), size: 6 },
    { name: 'randomIndex', address: addr('gRandomIndex'), size: 4 },
  ];
}

function inRam(a, n) {
  return (a >= 0x02000000 && a + n <= 0x02040000) || (a >= 0x03000000 && a + n <= 0x03008000);
}

function sample(memory, fields) {
  const ram = new DataView(memory);
  return Buffer.concat(fields.map(({ address, offset, size }) => {
    const base = offset === undefined ? address : ram.getUint32(address, true);
    const start = base + (offset ?? 0);
    if (!inRam(base, 1) || !inRam(start, size)) return Buffer.alloc(size);
    return Buffer.from(new Uint8Array(memory, start, size));
  }));
}

function compare(rom, port, fields, frames) {
  const size = fields.reduce((n, f) => n + f.size, 0);
  for (let tick = 0; tick < port.length; tick++) {
    const record = rom.subarray(tick * size, (tick + 1) * size);
    if (record.length !== size) break;
    if (record.equals(port[tick])) continue;
    const changes = {};
    for (let i = 0, offset = 0; i < fields.length; i++) {
      const { name, size } = fields[i];
      const a = port[tick].subarray(offset, offset + size), b = record.subarray(offset, offset + size);
      if (!a.equals(b)) changes[name] = { port: a.toString('hex'), rom: b.toString('hex') };
      offset += size;
    }
    return { tick, frame: frames[tick], changes };
  }
  if (rom.length !== port.length * size) return { portTicks: port.length, romTicks: rom.length / size };
  if (!port.length) return { error: 'No input ticks sampled' };
  return null;
}

function* tap(g, key, rest) {
  yield g.press(key, 2);
  yield g.press(0, rest);
}

function* swords(g) {
  const menu = g.lib.menuState;
  let field;
  for (let attempt = 0; attempt < 4; attempt++) {
    // Armor and story checkpoints can resume with a message covering the HUD.
    // Dismiss its pages and give the field time to accept Start.
    for (let page = 0; page < 40 && g.lib.message().open; page++) yield* tap(g, 'a', 8);
    field = menu()?.callback;
    if (field) {
      yield* tap(g, 'start', 60);
      if (menu() && menu().callback !== field) break;
    }
    yield g.press(0, 90);
  }
  if (!field || !menu() || menu().callback === field) return false;
  for (let i = 0; i < 8 && menu() && menu().page !== 4; i++) {
    yield* tap(g, menu().page > 4 ? 'l' : 'r', 40);
  }
  const reached = menu()?.page === 4;
  if (reached) {
    console.log(JSON.stringify({ swords: g.frameNo, state: menu() }));
    yield* tap(g, 'right', 28);
    yield* tap(g, 'a', 35);
    yield* tap(g, 'right', 28);
  }
  for (let i = 0; i < 6 && menu() && menu().callback !== field; i++) yield* tap(g, 'b', 35);
  return reached;
}

function random(upper) {
  seed ^= seed << 13;
  seed ^= seed >>> 17;
  seed ^= seed << 5;
  return (seed >>> 0) % upper;
}

function* fuzz(g) {
  const end = g.frameNo + frames;
  const field = g.lib.menuState()?.callback;
  while (g.frameNo < end) {
    const hud = g.lib.menuState();
    const menuKeys = ['a', 'b', 'left', 'right', 'up', 'down', 'l', 'r', 'start', 0];
    const fieldKeys = ['up', 'down', 'left', 'right', 'up+right', 'down+left', 'a', 'b', 'r', 'l', 'start', 0];
    const keys = hud && hud.callback === field && !g.lib.message().open ? fieldKeys : menuKeys;
    const key = keys[random(keys.length)];
    const hold = Math.min(end - g.frameNo, keys === fieldKeys ? 2 + random(47) : 2);
    yield g.press(key, hold);
    if (g.frameNo < end) yield g.press(0, Math.min(end - g.frameNo, 4 + random(22)));
  }
}
