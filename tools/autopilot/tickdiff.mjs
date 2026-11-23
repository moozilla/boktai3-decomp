#!/usr/bin/env node
// Port vs ROM, tick by tick: replays an autopilot movie on the port and its
// harness.txt on baserom.gba, dumps EWRAM and IWRAM at the entry of each game tick
// (Input_UpdatePadStates) in FROM..TO on both, and reports the first tick whose RAM
// differs beyond the noise floor (addresses that differ in the first ticks, where the runs
// are known to agree: the IWRAM stack, the port's own scratch). Each difference is named
// by global (types.json) and, for heap words, left as an address.
//
//   node tools/autopilot/tickdiff.mjs build/autopilot/run1 FROM TO [--base K] [--keep]
//   (--base K: the noise floor is every byte that differs in the first K ticks)
//
// Output: build/autopilot/tickdiff/{port,rom}.bin (deleted unless --keep) and a report.
import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';
import { openSession } from '../../port/agent/session.mjs';

const ROOT = path.resolve(path.dirname(new URL(import.meta.url).pathname), '../..');
const [dirArg, fromArg, toArg] = process.argv.slice(2);
const dir = path.resolve(dirArg), FROM = +fromArg, TO = +toArg;
const out = path.join(ROOT, 'build/autopilot/tickdiff');
fs.mkdirSync(out, { recursive: true });
const opt = (k, d) => { const i = process.argv.indexOf(k); return i < 0 ? d : process.argv[i + 1]; };
// --regions ADDR:LEN,... (hex) to dump less (default all of EWRAM and IWRAM)
const REG = opt('--regions', '2000000:40000,3000000:8000').split(',').map((x) => x.split(':').map((v) => parseInt(v, 16)));
const REC = REG.reduce((a, [, n]) => a + n, 0);
const run = JSON.parse(fs.readFileSync(path.join(dir, 'movie.json'), 'utf8'));

// ROM side: the run's harness script with a tickdump after "tick"
const script = fs.readFileSync(path.join(dir, 'harness.txt'), 'utf8').split('\n')
  .filter((l) => !/^(save |dump |shot |trace )/.test(l))
  .flatMap((l) => (/^tick /.test(l) ? ['trace off', l, `tickdump rom ${FROM}:${TO} ${REG.map(([a, n]) => `${a.toString(16)}:${n.toString(16)}`).join(',')}`] : [l]));
fs.writeFileSync(path.join(out, 'harness.txt'), script.join('\n'));
const r = spawnSync(path.join(ROOT, 'tools/emu/harness'), [path.join(ROOT, 'baserom.gba'), path.join(out, 'harness.txt'), out], { encoding: 'utf8', maxBuffer: 1 << 26 });
if (r.status) { console.error(r.stderr); process.exit(1); }

// Port side: hooks installed at boot see the replayed movie
const fd = fs.openSync(path.join(out, 'port.bin'), 'w');
let ticks = 0, s;
s = await openSession({
  movie: run.movie.slice(0, run.readFrames[Math.min(TO, run.readFrames.length - 1)] + 2), save: run.save || undefined, solarLog: run.solar, print: () => {},
  onBoot: (g) => g.on('Input_UpdatePadStates', () => {
    if (ticks >= FROM && ticks < TO) for (const [a, n] of REG) fs.writeSync(fd, new Uint8Array(s.memory.buffer, a, n));
    ticks++;
  }),
});
s.run(function* () { yield 1; });
fs.closeSync(fd);

// Compare
const P = fs.readFileSync(path.join(out, 'port.bin')), R = fs.readFileSync(path.join(out, 'rom.bin'));
const n = Math.min(P.length, R.length) / REC;
console.log(`ticks compared: ${n} (port ${P.length / REC}, rom ${R.length / REC})`);
const addrOf = (o) => { for (const [a, len] of REG) { if (o < len) return a + o; o -= len; } return -1; };
const globals = Object.entries(s.types.globals).map(([name, v]) => ({ name, addr: v.addr, size: v.size || 4 })).filter((x) => x.addr).sort((a, b) => a.addr - b.addr);
const nameOf = (a) => { let lo = 0, hi = globals.length - 1, best = null; while (lo <= hi) { const m = (lo + hi) >> 1; if (globals[m].addr <= a) { best = globals[m]; lo = m + 1; } else hi = m - 1; } return best && a < best.addr + Math.max(best.size, 4) ? `${best.name}+0x${(a - best.addr).toString(16)}` : `0x${a.toString(16)}`; };
// noise: bytes that differ at any of the first BASE ticks (where the runs agree)
const BASE = +opt('--base', 1);
const noise = new Set();
for (let t = 0; t < BASE; t++) for (let o = 0; o < REC; o++) if (P[t * REC + o] !== R[t * REC + o]) noise.add(o);
console.log(`noise floor over ticks ${FROM}..${FROM + BASE - 1}: ${noise.size} bytes`);
const dense = (offs) => { const runs = []; for (const o of offs) { const l = runs[runs.length - 1]; if (l && o - l[1] <= 4) l[1] = o; else runs.push([o, o]); } return runs; };
for (const [lo, hi] of dense([...noise].sort((a, b) => a - b)).slice(0, 40)) console.log(`  noise ${nameOf(addrOf(lo))} .. +${hi - lo + 1}`);
for (let t = BASE; t < n; t++) {
  const base = t * REC, diff = [];
  for (let o = 0; o < REC; o++) if (P[base + o] !== R[base + o] && !noise.has(o)) diff.push(o);
  if (!diff.length) continue;
  console.log(`first divergence at tick ${FROM + t} (frame ${run.readFrames[FROM + t]}): ${diff.length} bytes`);
  for (const [lo, hi] of dense(diff).slice(0, 60)) {
    const a = addrOf(lo), len = hi - lo + 1;
    const hx = (B) => [...B.subarray(base + lo, base + hi + 1)].map((x) => x.toString(16).padStart(2, '0')).join('');
    console.log(`  ${nameOf(a)} (0x${a.toString(16)}, ${len} B) port ${hx(P)} rom ${hx(R)}`);
  }
  break;
}
if (!process.argv.includes('--keep')) for (const f of ['port.bin', 'rom.bin']) fs.rmSync(path.join(out, f));
