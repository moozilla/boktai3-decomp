#!/usr/bin/env node
// Where does the port spin? Replays a run; from the entry of tick FROM it hooks every
// game function (enter and exit) and keeps the last RING calls, indented by depth.
// The ring is written to OUT at each tick and every 100000 calls, so when the replay
// stops advancing (an infinite loop in the game code) OUT ends with the frame that
// hangs: the loop's calls if it makes any, else what ran just before it.
//
//   node tools/autopilot/hang.mjs build/autopilot/run1 FROM OUT [RING] [FLUSH]
// (FLUSH: write the ring every FLUSH calls, default 100000; small values find a loop
// that makes few calls, at a cost in speed)
import fs from 'node:fs';
import path from 'node:path';
import { openSession } from '../../port/agent/session.mjs';

const ROOT = path.resolve(path.dirname(new URL(import.meta.url).pathname), '../..');
const [dir, fromArg, outPath, ringArg] = process.argv.slice(2);
const run = JSON.parse(fs.readFileSync(path.join(dir, 'movie.json'), 'utf8'));
const FROM = +fromArg, RING = +(ringArg || 4000), FLUSH = +(process.argv[6] || 100000);
const names = fs.readFileSync(path.join(ROOT, 'symbols/functions.csv'), 'utf8').split('\n').slice(1)
  .map((l) => l.split(',')).filter((c) => /^08[0-9A-F]{6}$/.test(c[0])).map((c) => c[1]);
let ticks = 0, calls = 0, ring = [];
const flush = (why) => fs.writeFileSync(outPath, `${why}: tick ${ticks - 1}, ${calls} calls since tick ${FROM}\n${ring.join('\n')}\n`);
const s = await openSession({
  movie: run.movie, save: run.save || undefined, print: () => {},
  onBoot: (g) => g.on('Input_UpdatePadStates', () => {
    ticks++;
    if (ticks - 1 > FROM) { flush('tick'); ring = []; }
    if (ticks - 1 === FROM) {
      for (const n of names) {
        for (const exit of [false, true]) {
          try {
            g.on(n, (a, info) => {
              ring.push(`${' '.repeat(Math.min(info.depth, 60))}${exit ? '<' : '>'} ${n} ${exit ? a[0].toString(16) : a.map((x) => x.toString(16)).join(',')}`);
              if (ring.length > RING) ring.splice(0, ring.length - RING);
              if (++calls % FLUSH === 0) flush('calls');
            }, { exit });
          } catch {}
        }
      }
    }
  }),
});
s.run(function* () { yield 1; });
flush('replay finished');
