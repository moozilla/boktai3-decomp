#!/usr/bin/env node
// Render an autopilot run to a video from its movie, with a small overlay: the
// current skill and goal along the bottom, the planned route as dots.
//
//   node tools/autopilot/video.mjs RUN_DIR/movie.json OUT.mp4 [--logs DIR,DIR...] [--seconds 60]
//        [--clips A-B:S,C-D:T,...]
//
// The movie is replayed on the port with the PPU on; every Nth frame (N chosen so
// the run fits --seconds at 30 fps) is piped as raw RGB into ffmpeg, so no frame
// files are written. Overlay text comes from the runs' log.json files (--logs:
// the run directories whose logs cover the movie; default: the movie's own).
// --clips: only frames A..B (S seconds of video each, the step chosen per clip), so a
// long run can show its highlights; the PPU only renders inside the clips.
import fs from 'node:fs';
import path from 'node:path';
import { spawn } from 'node:child_process';
import { openSession } from '../../port/agent/session.mjs';

const ROOT = path.resolve(path.dirname(new URL(import.meta.url).pathname), '../..');
const args = process.argv.slice(2);
const opt = (n, d) => { const i = args.indexOf(n); return i < 0 ? d : args[i + 1]; };
const moviePath = args[0], outPath = args[1];
const run = JSON.parse(fs.readFileSync(moviePath, 'utf8'));
const logDirs = opt('--logs', path.dirname(moviePath)).split(',');
const events = logDirs.flatMap((d) => { try { return JSON.parse(fs.readFileSync(path.join(d, 'log.json'), 'utf8')); } catch { return []; } })
  .sort((a, b) => a.frame - b.frame);
const seconds = +opt('--seconds', 60), fps = 30;
const clips = opt('--clips') ? opt('--clips').split(',').map((c) => { const [r, secs] = c.split(':'); const [a, b] = r.split('-').map(Number); return { a, b, step: Math.max(1, Math.ceil((b - a) / (+secs * fps))) }; })
  : [{ a: 0, b: run.movie.length, step: Math.max(1, Math.ceil(run.movie.length / (seconds * fps))) }];
const clipAt = (n) => clips.find((c) => n >= c.a && n < c.b);
let written = 0;

// 3x5 font: each glyph is 5 rows of 3 bits.
const F = {
  '0': '7555 7', '1': '2622 7', '2': '7174 7', '3': '7131 7', '4': '5571 1', '5': '7471 7', '6': '7475 7', '7': '7111 1', '8': '7575 7', '9': '7571 7',
  A: '2575 5', B: '6565 6', C: '3444 3', D: '6555 6', E: '7464 7', F: '7464 4', G: '3455 3', H: '5575 5', I: '7222 7', J: '1115 2', K: '5565 5', L: '4444 7', M: '5775 5',
  N: '6555 5', O: '2555 2', P: '6564 4', Q: '2557 3', R: '6565 5', S: '3421 6', T: '7222 2', U: '5555 7', V: '5555 2', W: '5577 5', X: '5525 5', Y: '5522 2', Z: '7124 7',
  ' ': '0000 0', '-': '0070 0', '>': '4212 4', ':': '0202 0', '.': '0000 2', ',': '0002 4', '/': '1124 4', '(': '1222 1', ')': '4222 4', '+': '0272 0', '=': '0707 0',
};
function text(px, x0, y0, s, color) {
  let x = x0;
  for (const ch of s.toUpperCase()) {
    const gph = (F[ch] || F[' ']).replace(' ', '');
    for (let r = 0; r < 5; r++) {
      const bits = parseInt(gph[r], 16);
      for (let c = 0; c < 3; c++) if (bits & (4 >> c)) dot(px, x + c, y0 + r, color);
    }
    x += 4;
    if (x > 236) break;
  }
}
function dot(px, x, y, [r, g, b]) {
  if (x < 0 || y < 0 || x >= 240 || y >= 160) return;
  const o = (y * 240 + x) * 3;
  px[o] = r; px[o + 1] = g; px[o + 2] = b;
}

const describe = (e) => {
  switch (e.skill) {
    case 'menu': if (e.goal) return `pause menu search: ${e.goal} ${e.done ? `found ${e.keys}` : 'not found'}`;
      return `menu ${e.menu.replace(/_Update|_Handle\w*/, '')} ${e.keys.map((k) => k.key || k).join(' ')}`;
    case 'narration': case 'dialogue': return `reading text ${e.string ?? ''}`;
    case 'goal': return `goal reached: ${e.reached}`;
    case 'room': return `room ${e.scene}: doors ${(e.doors || []).join(' ')}`;
    case 'navigate': return `door ${e.box}: ${e.reason}`;
    case 'interact': return `open ${e.item || 'object'}`;
    case 'equip': return `equip ${e.kind} ${e.name || e.id}`;
    case 'hit': return `hit ${e.with ? `with ${e.with}` : 'it'}: ${e.lit ? 'lit' : e.done ? 'done' : `${e.hits} hits, no effect`}`;
    case 'collect': return e.done ? 'picked up an item' : null;
    case 'heal': return `heal: life ${e.from} > ${e.to}`;
    case 'talk': return 'talk';
    case 'fought': return `fight ${e.won ? 'won' : 'left'} life ${e.life}`;
    case 'nav':
      if (e.push) return `push crate ${e.push.x},${e.push.z}`;
      if (e.pushPlan) return `plan pushes ${(e.pushPlan || []).join(' ')}`;
      if (e.searched) return `search found ${e.keys}`;
      if (e.fight) return `fight: enemy hp ${e.hp} life ${e.life}`;
      if (e.path) return `route ${e.path.map((p) => p.join(',')).join(' > ')}`;
      return null;
    default: return null;
  }
};

const ff = spawn('ffmpeg', ['-y', '-loglevel', 'error', '-f', 'rawvideo', '-pix_fmt', 'rgb24', '-s', '240x160', '-r', String(fps), '-i', '-',
  '-vf', 'scale=720:480:flags=neighbor', '-c:v', 'libx264', '-pix_fmt', 'yuv420p', '-crf', '20', outPath], { stdio: ['pipe', 'inherit', 'inherit'] });
const s = await openSession({ save: run.save || undefined, solarLog: run.solar, render: true, print: () => {}, lib: [path.join(ROOT, 'port/agent/lib')] });
const end = Math.min(run.movie.length, Math.max(...clips.map((c) => c.b)));
let ei = 0, caption = 'power on', route = null, routeScene = null;
const pending = [];
s.run(function* (g) {
  for (let n = 0; n < end; n++) {
    const clip = clipAt(n) || clipAt(n + 2);
    if (s.setRender) s.setRender(!!clip);
    yield g.press(run.movie[n] | 0, 1);
    while (ei < events.length && events[ei].frame <= n) {
      const e = events[ei++], d = describe(e);
      if (d) caption = d;
      if (e.skill === 'nav' && e.path) { route = e.path; routeScene = g.lib.where().scene; }
      if (e.skill === 'room' || e.skill === 'goal') route = null;
    }
    const c = clipAt(n);
    if (!c || (n - c.a) % c.step) continue;
    written++;
    const src = s.pixels(), px = Buffer.alloc(240 * 160 * 3);
    for (let i = 0; i < src.length; i++) {
      const c = src[i], r = c & 31, gg = (c >> 5) & 31, b = (c >> 10) & 31;
      px[3 * i] = (r << 3) | (r >> 2); px[3 * i + 1] = (gg << 3) | (gg >> 2); px[3 * i + 2] = (b << 3) | (b >> 2);
    }
    const p = g.$.gPlayers[0];
    if (route && p && g.lib.where().scene === routeScene) {
      for (const [cx, cz] of route) {
        const q = g.lib.project(cx * 256 + 128, p.position.y, cz * 256 + 128);
        for (const [dx, dy] of [[0, 0], [1, 0], [0, 1], [1, 1]]) dot(px, q.x + dx, q.y + dy, [255, 230, 40]);
      }
    }
    for (let y = 151; y < 160; y++) for (let x = 0; x < 240; x++) dot(px, x, y, [0, 0, 0]);
    text(px, 2, 153, caption.slice(0, 58), [255, 255, 255]);
    text(px, 200, 2, `F${n}`, [255, 255, 160]);
    pending.push(px);
    if (pending.length >= 16) { ff.stdin.write(Buffer.concat(pending)); pending.length = 0; }
  }
  if (pending.length) ff.stdin.write(Buffer.concat(pending));
});
ff.stdin.end();
await new Promise((res) => ff.on('close', res));
console.log(`${outPath}: ${Math.round(written / fps)} s, ${clips.map((c) => `${c.a}-${c.b} every ${c.step}`).join(', ')}`);
