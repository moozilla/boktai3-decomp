#!/usr/bin/env node
// Headless runner for the full-game module: node port/game/run_node.mjs
//   [--wasm build/port/game/game.wasm] [--rom baserom.gba] [--frames N]
//   [--shots 60,120,...] [--out build/port/game/frames] [--keys "300:start,420:a"]
// Writes frame_NNNNN.png for each requested frame. Keys: frame:button[+button]
// presses a button for 6 frames (a b select start right left up down r l).
import fs from 'node:fs';
import path from 'node:path';
import zlib from 'node:zlib';

const args = Object.fromEntries(process.argv.slice(2).reduce((acc, a, i, all) => {
  if (a.startsWith('--')) acc.push([a.slice(2), all[i + 1] && !all[i + 1].startsWith('--') ? all[i + 1] : '1']);
  return acc;
}, []));
const root = path.resolve(path.dirname(new URL(import.meta.url).pathname), '../..');
const wasmPath = args.wasm || path.join(root, 'build/port/game/game.wasm');
const romPath = args.rom || path.join(root, 'baserom.gba');
const maxFrames = Number(args.frames || 300);
const shots = new Set((args.shots || String(maxFrames)).split(',').map(Number));
const outDir = args.out || path.join(root, 'build/port/game/frames');
const BUTTONS = { a: 0, b: 1, select: 2, start: 3, right: 4, left: 5, up: 6, down: 7, r: 8, l: 9 };
const presses = (args.keys || '').split(',').filter(Boolean).map((s) => {
  const [f, b] = s.split(':');
  const [start, len] = f.split('+').map(Number);
  return { start, end: start + (len || 6), mask: b.split('&').reduce((m, k) => m | (1 << BUTTONS[k]), 0) };
});
fs.mkdirSync(outDir, { recursive: true });

function png(rgb555, file) {
  const w = 240, h = 160;
  const raw = Buffer.alloc((w * 3 + 1) * h);
  for (let y = 0; y < h; y++) {
    raw[y * (w * 3 + 1)] = 0;
    for (let x = 0; x < w; x++) {
      const c = rgb555[y * w + x];
      const o = y * (w * 3 + 1) + 1 + x * 3;
      const r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
      raw[o] = (r << 3) | (r >> 2); raw[o + 1] = (g << 3) | (g >> 2); raw[o + 2] = (b << 3) | (b >> 2);
    }
  }
  const crcTable = Array.from({ length: 256 }, (_, n) => { let c = n; for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1; return c >>> 0; });
  const crc = (buf) => { let c = 0xffffffff; for (const b of buf) c = crcTable[(c ^ b) & 255] ^ (c >>> 8); return (c ^ 0xffffffff) >>> 0; };
  const chunk = (type, data) => {
    const len = Buffer.alloc(4); len.writeUInt32BE(data.length);
    const td = Buffer.concat([Buffer.from(type), data]);
    const c = Buffer.alloc(4); c.writeUInt32BE(crc(td));
    return Buffer.concat([len, td, c]);
  };
  const ihdr = Buffer.alloc(13); ihdr.writeUInt32BE(w, 0); ihdr.writeUInt32BE(h, 4); ihdr[8] = 8; ihdr[9] = 2;
  fs.writeFileSync(file, Buffer.concat([Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]), chunk('IHDR', ihdr),
    chunk('IDAT', zlib.deflateSync(raw)), chunk('IEND', Buffer.alloc(0))]));
}

class Stop extends Error {}
if (args.stall) {
  // After N seconds, pause the main thread from a worker and print its stack.
  const { Worker } = await import('node:worker_threads');
  const w = new Worker(`
    const { workerData } = require('node:worker_threads');
    const inspector = require('node:inspector');
    const fs = require('node:fs');
    setTimeout(() => {
      const s = new inspector.Session();
      s.connectToMainThread();
      s.on('Debugger.paused', (m) => {
        const frames = m.params.callFrames.map((f) => f.functionName + ' @' + f.location.columnNumber);
        fs.writeSync(2, 'STALL STACK:\\n  ' + frames.join('\\n  ') + '\\n');
        process.kill(process.pid, 'SIGKILL');
      });
      s.post('Debugger.enable', () => s.post('Debugger.pause'));
    }, workerData * 1000);
  `, { eval: true, workerData: Number(args.stall) });
  w.unref();
}
let memory;
let audioFrames = 0;
const started = Date.now();
const cstr = (p) => { const b = new Uint8Array(memory.buffer, p); let e = 0; while (b[e]) e++; return Buffer.from(b.slice(0, e)).toString(); };
const imports = { env: {
  frame(ptr, n) {
    if (args.verbose) fs.writeSync(2, `frame ${n}\n`);
    if (shots.has(n)) png(new Uint16Array(memory.buffer, ptr, 240 * 160), path.join(outDir, `frame_${String(n).padStart(5, '0')}.png`));
    if (n >= maxFrames) {
      if (args.dump) for (const [name, a, len] of [['io', 0x04000000, 0x400], ['pal', 0x05000000, 0x400], ['vram', 0x06000000, 0x18000], ['oam', 0x07000000, 0x400], ['ewram', 0x02000000, 0x40000], ['iwram', 0x03000000, 0x8000]])
        fs.writeFileSync(path.join(outDir, `${name}.bin`), new Uint8Array(memory.buffer, a, len));
      if (instance.exports.port_trace_map && args.trace)
        fs.writeFileSync(args.trace, new Uint8Array(memory.buffer, instance.exports.port_trace_map(), 0x800000));
      throw new Stop();
    }
    let keys = 0;
    for (const p of presses) if (n >= p.start && n < p.end) keys |= p.mask;
    return keys;
  },
  audio(ptr, frames, rate) { audioFrames += frames; },
  hook() {}, // agent breakpoints (port/agent/); never armed here
  log(msg, value) { fs.writeSync(2, `[game] ${cstr(msg)} 0x${(value >>> 0).toString(16)}\n`); },
  fatal(msg, value) { fs.writeSync(2, `[fatal] ${cstr(msg)} 0x${(value >>> 0).toString(16)}\n`); },
} };
var { instance } = await WebAssembly.instantiate(fs.readFileSync(wasmPath), imports);
const ex = instance.exports;
memory = ex.memory;
if (ex.port_memory_init() !== 0) throw new Error('memory init failed');
const rom = fs.readFileSync(romPath);
new Uint8Array(memory.buffer, 0x08000000, rom.length).set(rom);
if (ex.port_load_rom(0x08000000, rom.length) !== 0) throw new Error('ROM load failed');
ex.port_game_set_time(Math.floor(Date.parse(args.time || '2025-11-13T12:00:00Z') / 1000));
let saveSize = 0;
if (args.save) {
  const save = fs.readFileSync(args.save);
  new Uint8Array(memory.buffer, ex.port_game_save_stage(), save.length).set(save);
  saveSize = save.length;
}
try {
  const r = ex.port_game_run(saveSize);
  console.log('port_game_run returned', r);
} catch (e) {
  if (!(e instanceof Stop)) {
    console.error('stopped at frame', Number(ex.port_frame_count()), e.stack.split('\n').slice(0, 14).join('\n'));
    process.exitCode = 1;
  }
}
const secs = (Date.now() - started) / 1000;
console.log(`frames: ${Number(ex.port_frame_count())} in ${secs.toFixed(1)}s (${(Number(ex.port_frame_count()) / secs).toFixed(0)} fps), audio frames ${audioFrames}`);
