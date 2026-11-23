#!/usr/bin/env node
// PNG of the port's screen at frames of a run: node tools/autopilot/shot.mjs RUN_DIR OUT_PREFIX FRAME...
// (frames past the movie's end run with no keys).
import fs from 'node:fs';
import path from 'node:path';
import { openSession, png } from '../../port/agent/session.mjs';

const [dir, prefix, ...frames] = process.argv.slice(2);
const run = JSON.parse(fs.readFileSync(path.join(dir, 'movie.json'), 'utf8'));
const want = frames.map(Number).sort((a, b) => a - b), first = want[0];
const s = await openSession({ movie: run.movie.slice(0, Math.min(first - 2, run.movie.length)), save: run.save || undefined, solarLog: run.solar, print: () => {} });
s.run(function* (g) {
  s.setRender(true);
  for (const f of want) {
    while (s.frame < f) yield { frames: 1, keys: run.movie[s.frame + 1] | 0 };
    fs.writeFileSync(`${prefix}_${f}.png`, png(s.pixels()));
    console.log(`${prefix}_${f}.png`, JSON.stringify(g.lib.where()));
  }
});
