#!/usr/bin/env node
// Inspect the port at a tick of a recorded run: replays the movie up to the entry of
// game tick N (Input_UpdatePadStates) and evaluates JS there with g, $ and lib.
//
//   node tools/autopilot/at.mjs build/autopilot/run1 TICK 'lib.taskOf(0x0200f3a4)'
import fs from 'node:fs';
import path from 'node:path';
import { openSession } from '../../port/agent/session.mjs';

const ROOT = path.resolve(path.dirname(new URL(import.meta.url).pathname), '../..');
const [dir, tickArg, ...code] = process.argv.slice(2);
const run = JSON.parse(fs.readFileSync(path.join(dir, 'movie.json'), 'utf8'));
const TICK = +tickArg;
let ticks = 0, result;
const s = await openSession({
  movie: run.movie.slice(0, run.readFrames[TICK] + 1), save: run.save || undefined, print: () => {},
  lib: [path.join(ROOT, 'port/agent/lib'), path.join(ROOT, 'tools/autopilot/lib')],
  onBoot: (g) => g.on('Input_UpdatePadStates', () => {
    if (ticks++ === TICK) {
      const lib = g.lib, $ = g.$;
      for (const c of code) {
        try { result = eval(c); } catch (e) { result = String(e); }
        console.log(JSON.stringify(result, (k, v) => (typeof v === 'bigint' ? String(v) : k === 'view' ? undefined : v), 1));
      }
    }
  }),
});
s.run(function* () { yield 1; });
