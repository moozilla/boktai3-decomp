#!/usr/bin/env node
// Try keys at the end of a recorded run and look: replays RUN/movie.json, then runs the
// steps given (for exploring a menu or a skill by hand before writing it down):
//
//   node tools/autopilot/poke.mjs RUN_DIR OUT_PREFIX 'start' 'wait 40' 'shot menu' \
//        'right' 'eval lib.tasks().filter(t=>!t.dead).map(t=>t.update)' ...
//
// A step is a key name (pressed 2 frames, then 12 frames released; 'select+r' holds
// both), 'hold KEYS N', 'wait N', 'shot NAME' (PNG OUT_PREFIX_NAME.png) or 'eval JS'
// (g, $, lib in scope; printed as JSON) or 'do SKILL(...)' (a skill generator, run to its end).
import fs from 'node:fs';
import path from 'node:path';
import { openSession, png } from '../../port/agent/session.mjs';

const ROOT = path.resolve(path.dirname(new URL(import.meta.url).pathname), '../..');
const [dir, prefix, ...steps] = process.argv.slice(2);
const run = JSON.parse(fs.readFileSync(path.join(dir, 'movie.json'), 'utf8'));
const s = await openSession({ movie: run.movie, save: run.save || undefined, solarLog: run.solar, print: () => {},
  lib: [path.join(ROOT, 'port/agent/lib'), path.join(ROOT, 'tools/autopilot/lib')] });
try { s.run(function* (g) {
  const lib = g.lib, $ = g.$;
  lib.session = s;
  s.setRender(true);
  for (const st of steps) {
    const [cmd, ...rest] = st.split(' ');
    if (cmd === 'wait') yield g.press(0, +rest[0]);
    else if (cmd === 'hold') yield g.press(rest[0], +rest[1]);
    else if (cmd === 'shot') { yield 1; fs.writeFileSync(`${prefix}_${rest[0]}.png`, png(s.pixels())); console.log('shot', rest[0]); }
    else if (cmd === 'trace') {  // 'trace OUT': from here, the last calls (enter/exit) go to OUT every 200 calls
      const names = fs.readFileSync(path.join(ROOT, 'symbols/functions.csv'), 'utf8').split('\n').slice(1)
        .map((l) => l.split(',')).filter((c) => /^08[0-9A-F]{6}$/.test(c[0])).map((c) => c[1]);
      let ring = [], calls = 0;
      for (const n of names) for (const exit of [false, true]) {
        try { g.on(n, (a, info) => { ring.push(`${' '.repeat(Math.min(info.depth, 60))}${exit ? '<' : '>'} ${n} ${exit ? (a[0] >>> 0).toString(16) : a.map((x) => (x >>> 0).toString(16)).join(',')}`); if (ring.length > 3000) ring.splice(0, 1000); if (++calls % 1 === 0) fs.writeFileSync(rest[0], ring.join('\n')); }, { exit }); } catch {}
      }
    } else if (cmd === 'do') {  // a skill: 'do lib.quickChange({ gun: true })'
      let r; try { r = yield* eval(rest.join(' ')); } catch (e) { r = String(e.stack || e); }
      console.log(JSON.stringify(r, (k, v) => (typeof v === 'bigint' ? String(v) : v)));
    } else if (cmd === 'eval') {
      let r; try { r = eval(rest.join(' ')); } catch (e) { r = String(e); }
      console.log(JSON.stringify(r, (k, v) => (typeof v === 'bigint' ? String(v) : v)));
    } else { yield g.press(cmd, 2); yield g.press(0, 12); }
  }
}); } catch (e) { console.log(String(e.stack || e)); }
