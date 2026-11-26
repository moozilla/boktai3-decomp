#!/usr/bin/env node
// Port vs ROM, tick by tick: replays an autopilot movie on the port and its
// harness.txt on baserom.gba, dumps EWRAM and IWRAM at the entry of each game tick
// (Input_UpdatePadStates) in FROM..TO on both, and reports the first differing keys
// or RAM bytes beyond the noise floor (bytes that differ in the first ticks from the stack
// or the port's own scratch). Each RAM difference is named
// by global (types.json) and, for heap words, left as an address.
//
//   node tools/autopilot/tickdiff.mjs build/autopilot/run1 FROM TO [--base K] [--keep]
//   (--base K: the noise floor is every byte that differs in the first K ticks)
//
// Output: build/autopilot/tickdiff/{port,rom}.bin (deleted unless --keep) and a report.
import fs from 'node:fs';
import path from 'node:path';
import { setFlagsFromString } from 'node:v8';
import { openSession } from '../../port/agent/session.mjs';
import { clock, lux, runRom } from './lib/rom.mjs';
import { readPressedKeys } from './lib/tick-samples.mjs';

setFlagsFromString('--wasm-enforce-bounds-checks');

const ROOT = path.resolve(path.dirname(new URL(import.meta.url).pathname), '../..');
const [dirArg, fromArg, toArg] = process.argv.slice(2);
const dir = path.resolve(dirArg), FROM = +fromArg, TO = +toArg;
if (!Number.isSafeInteger(FROM) || !Number.isSafeInteger(TO) || FROM < 0 || TO <= FROM)
  throw new Error('Expected a nonempty tick range FROM TO');
const opt = (k, d) => { const i = process.argv.indexOf(k); return i < 0 ? d : process.argv[i + 1]; };
const out = path.resolve(opt('--out', path.join(ROOT, 'build/autopilot/tickdiff')));
fs.mkdirSync(out, { recursive: true });
// --regions ADDR:LEN,... (hex) to dump less (default all of EWRAM and IWRAM)
const REG = opt('--regions', '2000000:40000,3000000:8000').split(',').map((x) => x.split(':').map((v) => parseInt(v, 16)));
const REC = REG.reduce((a, [, n]) => a + n, 0);
const ROM_REC = REC + 2;
const spec = REG.map(([address, bytes]) => `${address.toString(16)}:${bytes.toString(16)}`).join(',');
const run = JSON.parse(fs.readFileSync(path.join(dir, 'movie.json'), 'utf8'));
if (TO > run.readFrames.length)
  throw new Error(`Movie has only ${run.readFrames.length} sampled ticks`);

// ROM side: the run's harness script with a tickdump after "tick"
const lines = fs.readFileSync(path.join(dir, 'harness.txt'), 'utf8').split('\n')
  .filter((l) => !/^(save |dump |shot |trace |tickdump )/.test(l))
  .flatMap((l) => (/^(fast)?tick /.test(l) ?
    ['trace off', l, `tickdump rom ${FROM}:${TO} ${spec},keyinput`] : [l]));
// Once the window has been captured, no later inputs can affect the comparison.
const script = [];
let left = TO;
for (const line of lines) {
  const hold = /^holdt (\S+) (\d+)$/.exec(line);
  if (!hold) {
    script.push(line);
    continue;
  }
  const n = Math.min(left, Number(hold[2]));
  script.push(`holdt ${hold[1]} ${n}`);
  left -= n;
  if (!left) break;
}
fs.writeFileSync(path.join(out, 'harness.txt'), script.join('\n') + '\n');
runRom(path.join(out, 'harness.txt'), out, { save: run.save, render: false, timeout: 600_000 });

// Port side: hooks installed at boot see the replayed movie
let ticks = 0, s;
let fd;
const portKeys = [], portFrames = [];
s = await openSession({
  movie: run.movie.slice(0, run.readFrames[TO - 1] + 1), save: run.save || undefined,
  wasm: opt('--wasm'), solarLog: run.solar, solar: lux, time: clock, print: () => {},
  onBoot: (g) => g.on('Input_UpdatePadStates', () => {
    if (ticks >= FROM && ticks < TO) {
      const memory = s.memory.buffer;
      portKeys.push(readPressedKeys(new DataView(memory)));
      portFrames.push(s.frame);
      for (const [a, n] of REG) fs.writeSync(fd, new Uint8Array(memory, a, n));
    }
    ticks++;
  }),
});
fd = fs.openSync(path.join(out, 'port.bin'), 'w');
try {
  s.run(function* () {});
} catch (error) {
  if (!run.fault || ticks !== TO || !(error instanceof WebAssembly.RuntimeError)) throw error;
  console.warn(`Port trapped after sampling the requested ticks: ${error.message}`);
} finally {
  fs.closeSync(fd);
}

// Compare
const P = fs.readFileSync(path.join(out, 'port.bin')), R = fs.readFileSync(path.join(out, 'rom.bin'));
const portCount = P.length / REC, romCount = R.length / ROM_REC;
const n = Math.min(portCount, romCount);
console.log(`ticks compared: ${n} (port ${portCount}, rom ${romCount})`);
if (portCount !== TO - FROM || romCount !== TO - FROM) throw new Error('Incomplete tick dump');
const romMemory = new DataView(R.buffer, R.byteOffset, R.byteLength);
const addrOf = (o) => { for (const [a, len] of REG) { if (o < len) return a + o; o -= len; } return -1; };
const globals = Object.entries(s.types.globals).map(([name, v]) => ({ name, addr: v.addr, size: v.size || 4 })).filter((x) => x.addr).sort((a, b) => a.addr - b.addr);
const nameOf = (a) => { let lo = 0, hi = globals.length - 1, best = null; while (lo <= hi) { const m = (lo + hi) >> 1; if (globals[m].addr <= a) { best = globals[m]; lo = m + 1; } else hi = m - 1; } return best && a < best.addr + Math.max(best.size, 4) ? `${best.name}+0x${(a - best.addr).toString(16)}` : `0x${a.toString(16)}`; };
// noise: bytes that differ at any of the first BASE ticks (where the runs agree)
const BASE = +opt('--base', 1);
if (!Number.isSafeInteger(BASE) || BASE < 0 || BASE >= n)
  throw new Error('--base must leave at least one tick to compare');
const noise = new Set();
for (let t = 0; t < BASE; t++) {
  for (let o = 0; o < REC; o++) if (P[t * REC + o] !== R[t * ROM_REC + o]) noise.add(o);
}
if (BASE) console.log(`noise floor over ticks ${FROM}..${FROM + BASE - 1}: ${noise.size} bytes`);
const dense = (offs) => { const runs = []; for (const o of offs) { const l = runs[runs.length - 1]; if (l && o - l[1] <= 4) l[1] = o; else runs.push([o, o]); } return runs; };
for (const [lo, hi] of dense([...noise].sort((a, b) => a - b)).slice(0, 40)) console.log(`  noise ${nameOf(addrOf(lo))} .. +${hi - lo + 1}`);
for (let t = 0; t < n; t++) {
  const portOffset = t * REC, romOffset = t * ROM_REC, diff = [];
  if (t >= BASE) {
    for (let o = 0; o < REC; o++) if (P[portOffset + o] !== R[romOffset + o] && !noise.has(o)) diff.push(o);
  }
  const romKeys = readPressedKeys(romMemory, romOffset + REC);
  const keyDiffers = portKeys[t] !== romKeys;
  if (!diff.length && !keyDiffers) continue;
  console.log(`first divergence at tick ${FROM + t} (frame ${portFrames[t]}): ${diff.length} bytes`);
  if (keyDiffers)
    console.log(`  KEYINPUT pressed mask port 0x${portKeys[t].toString(16)} rom 0x${romKeys.toString(16)}`);
  for (const [lo, hi] of dense(diff).slice(0, 60)) {
    const a = addrOf(lo), len = hi - lo + 1;
    const hx = (B, base) => B.subarray(base + lo, base + hi + 1).toString('hex');
    console.log(`  ${nameOf(a)} (0x${a.toString(16)}, ${len} B) port ${hx(P, portOffset)} rom ${hx(R, romOffset)}`);
  }
  break;
}
if (process.argv.includes('--keep')) {
  const ram = Buffer.alloc(portCount * REC);
  for (let t = 0; t < n; t++) R.copy(ram, t * REC, t * ROM_REC, t * ROM_REC + REC);
  fs.writeFileSync(path.join(out, 'rom.bin'), ram);
} else {
  for (const f of ['port.bin', 'rom.bin']) fs.rmSync(path.join(out, f));
}
