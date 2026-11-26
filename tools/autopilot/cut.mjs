#!/usr/bin/env node
// Cut a run's movie at a frame, to start another run there (--from OUT/movie.json).
//   node tools/autopilot/cut.mjs build/autopilot/s3 33900 build/autopilot/at2776
import fs from 'node:fs';
import path from 'node:path';
import { tickAt } from './movie-index.mjs';

const [dir, frameArg, out] = process.argv.slice(2);
const run = JSON.parse(fs.readFileSync(path.join(dir, 'movie.json'), 'utf8'));
const F = +frameArg;
fs.mkdirSync(out, { recursive: true });
const cut = { ...run, frames: F, movie: run.movie.slice(0, F), readFrames: run.readFrames.slice(0, tickAt(run.readFrames, F)),
  solar: (run.solar || []).filter(([n]) => n < F),
  rooms: (run.rooms || []).filter((r) => r.frame < F), end: undefined, tried: undefined, visited: undefined, cutFrom: path.relative(process.cwd(), dir) };
fs.writeFileSync(path.join(out, 'movie.json'), JSON.stringify(cut));
console.log(`${out}/movie.json: ${F} frames, ${cut.readFrames.length} input reads, rooms ${cut.rooms.map((r) => r.scene).join(' ')}`);
