#!/usr/bin/env node
// Flight recorder host for the source port's --record build
// (build/port/game-rec/game.wasm; tools/recorder/README.md).
//
//   node tools/recorder/runner.mjs record OUT [--save F.sav] [--time ISO] [--solar N]
//        [--script FILE.js | --code 'JS'] [--frames N] [--every 60] [--screens 30]
//   node tools/recorder/runner.mjs serve SESSION     JSON commands on stdin, one per line
//
// record runs the game from power-on (or a battery save) under a script in the
// agent's JavaScript dialect (port/agent/README.md: `yield N`, `yield g.press(..)`,
// skills from port/agent/lib) and writes the session directory:
//   meta.json         start conditions, intervals, layout of the files below
//   frames.bin        per frame: frame, keys, solar, flags, RAM hash (lo, hi), write words
//   writes.bin        the write log: 16-byte rows [frame, site, address | wcode << 28, value]
//                     (copies and fills become word rows, then byte rows for a tail)
//   snap/NNNNNNNN.gz  module memory at the end of that frame (snapshots)
//   screens/NNNNNNNN.rgb  the frame buffer (RGB555) every S frames
//   edits.json        memory the script changed between frames (re-applied on replay)
// serve restores snapshots in place at a frame boundary and re-runs frames with the
// recorded inputs, checking every frame's RAM hash; commands (JSON):
//   {"op":"seek","frame":F}                 machine at the end of frame F
//   {"op":"run","frames":N,"trace":DIR?}    run on (trace mode writes DIR/fNNNNNNNN.bin)
//   {"op":"solar","level":N} / {"op":"keys","mask":M} / {"op":"poke","addr":A,"size":S,"value":V}
//                                            perturb (the hash check then reports divergence)
//   {"op":"read","addr":A,"len":L}          bytes (hex)
//   {"op":"ram","file":PATH}                EWRAM + IWRAM to a file
//   {"op":"shot","file":PATH}               PNG of the current frame
//   {"op":"status"}  {"op":"quit"}
import fs from 'node:fs';
import path from 'node:path';
import zlib from 'node:zlib';
import { createGame } from '../../port/agent/gameapi.mjs';

const ROOT = path.resolve(path.dirname(new URL(import.meta.url).pathname), '../..');
const WASM = process.env.REC_WASM || path.join(ROOT, 'build/port/game-rec/game.wasm');
// ---- event parsing (formats in tools/recorder/rec_rt.c) ----
class Rows {
  constructor(cols) { this.cols = cols; this.buf = new Uint32Array(cols * 65536); this.n = 0; }
  push(...v) {
    if ((this.n + 1) * this.cols > this.buf.length) { const b = new Uint32Array(this.buf.length * 2); b.set(this.buf); this.buf = b; }
    this.buf.set(v, this.n * this.cols); this.n++;
  }
  bytes() { return Buffer.from(this.buf.buffer, 0, this.n * this.cols * 4); }
}

function bytesRows(out, frame, site, dst, words, length) {
  // data words of a copy or fill -> word rows, then byte rows for the tail
  let k = 0;
  for (; k + 4 <= length; k += 4) out.push(frame, site, (dst + k) | 2 << 28, words(k >> 2));
  for (; k < length; k++) out.push(frame, site, dst + k, (words(k >> 2) >>> (8 * (k & 3))) & 0xff);
}

// record mode: stores only, as write rows
function parseWrites(w, frame, out) {
  for (let i = 0; i < w.length;) {
    const s = w[i], kind = s >>> 29;
    switch (kind) {
      case 0: i += 1; break;
      case 7: i += 2; break;
      case 1: i += 3; break;
      case 2: out.push(frame, s, w[i + 1], w[i + 2]); i += 3; break;
      case 3: {
        const a = w[i + 1];
        if (a >>> 31) { const base = a & 0x0fffffff; out.push(frame, s, base | 2 << 28, w[i + 2]); out.push(frame, s, (base + 4) | 2 << 28, w[i + 3]); }
        i += 4; break;
      }
      case 4: { const dst = w[i + 1], len = w[i + 3], at = i + 4; bytesRows(out, frame, s, dst, (j) => w[at + j], len); i += 4 + ((len + 3) >>> 2); break; }
      case 5: { const b = w[i + 2] & 0xff, word = (b | b << 8 | b << 16 | b << 24) >>> 0; bytesRows(out, frame, s, w[i + 1], () => word, w[i + 3]); i += 4; break; }
      case 6: {
        const size = w[i + 2] & 0x7fffffff, at = i + 3;
        if (w[i + 2] >>> 31) bytesRows(out, frame, s, w[i + 1], (j) => w[at + j], size);
        i += 3 + ((size + 3) >>> 2); break;
      }
    }
  }
}

// trace mode: one 5-word row per event [site, a, b, c, invocation]
function makeTracer() {
  const stack = [];
  let next = 1, synthetic = 0x80000000, current = synthetic++;
  return function parseTrace(w, out) {
    for (let i = 0; i < w.length;) {
      const s = w[i], kind = s >>> 29;
      switch (kind) {
        case 0: out.push(s, 0, 0, 0, current); i += 1; break;
        case 7:
          if (w[i + 1] === 0) { stack.push(current); const parent = current; current = next++; out.push(s, 0, parent, 0, current); }
          else { out.push(s, 1, 0, 0, current); current = stack.length ? stack.pop() : synthetic++; }
          i += 2; break;
        case 1: case 2: out.push(s, w[i + 1], w[i + 2], 0, current); i += 3; break;
        case 3: out.push(s, w[i + 1], w[i + 2], w[i + 3], current); i += 4; break;
        case 4: out.push(s, w[i + 1], w[i + 2], w[i + 3], current); i += 4 + ((w[i + 3] + 3) >>> 2); break;
        case 5: out.push(s, w[i + 1], w[i + 2], w[i + 3], current); i += 4; break;
        case 6: out.push(s, w[i + 1], w[i + 2], 0, current); i += 3 + (((w[i + 2] & 0x7fffffff) + 3) >>> 2); break;
      }
    }
  };
}

function words(buffers) {
  const total = buffers.reduce((n, b) => n + b.length, 0);
  const all = Buffer.concat(buffers, total);
  return new Uint32Array(all.buffer, all.byteOffset, total / 4);
}
const REGIONS = [[0x02000000, 0x40000], [0x03000000, 0x8000], [0x04000000, 0x400], [0x05000000, 0x400],
  [0x06000000, 0x18000], [0x07000000, 0x400]];
const MODE_REC = 1, MODE_TRACE = 2;

class Stop extends Error {}

function parseArgs(list) {
  const out = { _: [] };
  for (let i = 0; i < list.length; i++) {
    const a = list[i];
    if (a.startsWith('--')) {
      const next = list[i + 1];
      if (next === undefined || next.startsWith('--')) out[a.slice(2)] = true;
      else { out[a.slice(2)] = next; i++; }
    } else out._.push(a);
  }
  return out;
}

function png(rgb555, file) {
  const w = 240, h = 160;
  const raw = Buffer.alloc((w * 3 + 1) * h);
  for (let y = 0; y < h; y++) {
    raw[y * (w * 3 + 1)] = 0;
    for (let x = 0; x < w; x++) {
      const c = rgb555[y * w + x], o = y * (w * 3 + 1) + 1 + x * 3;
      const r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
      raw[o] = (r << 3) | (r >> 2); raw[o + 1] = (g << 3) | (g >> 2); raw[o + 2] = (b << 3) | (b >> 2);
    }
  }
  const table = png.table || (png.table = Array.from({ length: 256 }, (_, n) => {
    let c = n; for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1; return c >>> 0;
  }));
  const crc = (b) => { let c = 0xffffffff; for (const x of b) c = table[(c ^ x) & 255] ^ (c >>> 8); return (c ^ 0xffffffff) >>> 0; };
  const chunk = (t, d) => {
    const l = Buffer.alloc(4); l.writeUInt32BE(d.length);
    const td = Buffer.concat([Buffer.from(t), d]); const c = Buffer.alloc(4); c.writeUInt32BE(crc(td));
    return Buffer.concat([l, td, c]);
  };
  const ih = Buffer.alloc(13); ih.writeUInt32BE(w, 0); ih.writeUInt32BE(h, 4); ih[8] = 8; ih[9] = 2;
  fs.writeFileSync(file, Buffer.concat([Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]), chunk('IHDR', ih),
    chunk('IDAT', zlib.deflateSync(raw, { level: 1 })), chunk('IEND', Buffer.alloc(0))]));
}

// ---------------------------------------------------------------------------
// The machine: the wasm instance and its frame boundary.
async function boot({ save, time, solar, onFrame, onFlush }) {
  let memory, instance;
  const imports = { env: {
    frame(ptr, n) { return onFrame(ptr, n) >>> 0; },
    audio() {},
    hook(kind, address) { if (machine.g) machine.g.dispatch(kind, address); },
    log(msg, value) { process.stderr.write(`[game] ${cstr(msg)} 0x${(value >>> 0).toString(16)}\n`); },
    fatal(msg, value) { process.stderr.write(`[fatal] ${cstr(msg)} 0x${(value >>> 0).toString(16)}\n`); },
    rec_flush() { onFlush(); },
  } };
  const cstr = (p) => { const b = new Uint8Array(memory.buffer); let e = p; while (b[e]) e++; return Buffer.from(b.slice(p, e)).toString(); };
  ({ instance } = await WebAssembly.instantiate(fs.readFileSync(WASM), imports));
  const ex = instance.exports;
  memory = ex.memory;
  if (ex.port_memory_init() !== 0) throw new Error('memory init failed');
  const rom = fs.readFileSync(path.join(ROOT, 'baserom.gba'));
  new Uint8Array(memory.buffer, 0x08000000, rom.length).set(rom);
  if (ex.port_load_rom(0x08000000, rom.length) !== 0) throw new Error('ROM load failed');
  ex.port_game_set_time(Math.floor(Date.parse(time) / 1000));
  ex.port_game_set_solar(solar);
  let saveSize = 0;
  if (save) {
    const data = fs.readFileSync(save);
    new Uint8Array(memory.buffer, ex.port_game_save_stage(), data.length).set(data);
    saveSize = data.length;
  }
  const machine = { ex, memory, saveSize, g: null };
  machine.heapBase = ex.__heap_base.value;
  machine.stackPointer = () => (ex.__stack_pointer ? ex.__stack_pointer.value : 0);
  machine.rec = ex.rec_state();
  // static-data writes are logged for every unit except the emulated hardware (rec_rt.c)
  const units = JSON.parse(fs.readFileSync(path.join(path.dirname(WASM), 'rec/units.json'), 'utf8'));
  const bits = new Uint8Array(memory.buffer, ex.rec_static_units(), 1024);
  units.forEach((u, i) => { if (!/^port\/(hw|core)\//.test(u)) bits[i >> 3] |= 1 << (i & 7); });
  return machine;
}

function ramHash(memory) {
  // 64-bit FNV-style hash over EWRAM then IWRAM, as two 32-bit halves
  const words = new Uint32Array(memory.buffer, 0x02000000, 0x40000 / 4);
  let a = 0x811c9dc5 | 0, b = 0x01000193 | 0;
  for (let i = 0; i < words.length; i++) { a = Math.imul(a ^ words[i], 0x01000193); b = Math.imul(b ^ words[i] ^ (a >>> 15), 0x85ebca6b); }
  const iw = new Uint32Array(memory.buffer, 0x03000000, 0x8000 / 4);
  for (let i = 0; i < iw.length; i++) { a = Math.imul(a ^ iw[i], 0x01000193); b = Math.imul(b ^ iw[i] ^ (a >>> 15), 0x85ebca6b); }
  return [a >>> 0, b >>> 0];
}

function snapshotBuffers(machine) {
  const parts = [[0, machine.heapBase], ...REGIONS];
  return parts.map(([a, n]) => [a, Buffer.from(new Uint8Array(machine.memory.buffer, a, n))]);
}

function encodeSnapshot(parts, sp) {
  const header = Buffer.alloc(8 + parts.length * 8);
  header.writeUInt32LE(parts.length, 0);
  header.writeUInt32LE(sp >>> 0, 4);
  parts.forEach(([a, b], i) => { header.writeUInt32LE(a, 8 + i * 8); header.writeUInt32LE(b.length, 12 + i * 8); });
  return zlib.gzipSync(Buffer.concat([header, ...parts.map((p) => p[1])]), { level: 1 });
}

function decodeSnapshot(buf) {
  const raw = zlib.gunzipSync(buf);
  const n = raw.readUInt32LE(0), sp = raw.readUInt32LE(4);
  let off = 8 + n * 8;
  const parts = [];
  for (let i = 0; i < n; i++) {
    const a = raw.readUInt32LE(8 + i * 8), len = raw.readUInt32LE(12 + i * 8);
    parts.push([a, raw.subarray(off, off + len)]);
    off += len;
  }
  return { parts, sp };
}

function restore(machine, snap) {
  if (machine.stackPointer() !== snap.sp)
    throw new Error(`snapshot taken at another call depth (stack pointer ${snap.sp.toString(16)} vs ${machine.stackPointer().toString(16)})`);
  const bytes = new Uint8Array(machine.memory.buffer);
  for (const [a, b] of snap.parts) bytes.set(b, a);
}

function drain(machine, sink) {
  const dv = new DataView(machine.memory.buffer);
  const st = machine.rec;
  const count = dv.getUint32(st + 4, true);
  if (count) {
    const ring = dv.getUint32(st + 16, true);
    sink(Buffer.from(new Uint8Array(machine.memory.buffer, ring, count * 4)));
    dv.setUint32(st + 4, 0, true);
  }
}

function setMode(machine, mode) { new DataView(machine.memory.buffer).setUint32(machine.rec, mode, true); }

function loadTypes() {
  const p = path.join(ROOT, 'build/port/agent/types.json');
  return fs.existsSync(p) ? JSON.parse(fs.readFileSync(p, 'utf8')) : { types: {}, globals: {}, functions: {}, data: {} };
}

function attachScripts(machine) {
  const g = createGame({ memory: machine.memory, exports: machine.ex, types: loadTypes(), print: (t) => process.stderr.write(t + '\n') });
  const lib = path.join(ROOT, 'port/agent/lib');
  if (fs.existsSync(lib)) {
    const files = fs.readdirSync(lib).filter((f) => f.endsWith('.js')).sort();
    g.loadLib(Object.fromEntries(files.map((f) => [f, fs.readFileSync(path.join(lib, f), 'utf8')])));
  }
  machine.g = g;
  return g;
}

// ---------------------------------------------------------------------------
async function record(args) {
  const out = path.resolve(args._[1]);
  fs.mkdirSync(path.join(out, 'snap'), { recursive: true });
  fs.mkdirSync(path.join(out, 'screens'), { recursive: true });
  const every = Number(args.every || 60), screens = Number(args.screens || 30);
  const meta = { version: 1, save: args.save ? path.relative(ROOT, path.resolve(args.save)) : null,
    time: args.time || '2025-11-13T12:00:00Z', solar: Number(args.solar ?? 232), every, screens,
    script: args.script ? path.relative(ROOT, path.resolve(args.script)) : (args.code || null),
    wasm: path.relative(ROOT, WASM), wasmSize: fs.statSync(WASM).size, frameRow: 32 };
  const code = args.script ? fs.readFileSync(args.script, 'utf8') : (args.code || '');
  const maxFrames = Number(args.frames || 0);
  const framesFd = fs.openSync(path.join(out, 'frames.bin'), 'w');
  const writesFd = fs.openSync(path.join(out, 'writes.bin'), 'w');
  const edits = [];
  let machine, g = null, task = null, solar = meta.solar, prevRam = null, writeWords = 0, started = Date.now();
  const pending = [];
  const flush = () => drain(machine, (b) => pending.push(b));
  const onFrame = (ptr, n) => {
    // the frame that just ended is n; its writes are in the ring
    flush();
    const rows = new Rows(4);
    parseWrites(words(pending), n, rows);
    pending.length = 0;
    fs.writeSync(writesFd, rows.bytes());
    const total = rows.n;
    writeWords += total;
    // inputs for the next frame come from the script, which may also edit memory
    if (!g) {
      g = attachScripts(machine);
      if (code) task = g.spawn(g.compile(code), { name: 'session' });
    }
    const before = ramHash(machine.memory);
    const snapRam = Buffer.from(new Uint8Array(machine.memory.buffer, 0x02000000, 0x40000));
    const snapIw = Buffer.from(new Uint8Array(machine.memory.buffer, 0x03000000, 0x8000));
    const keys = g.frame(n + 1) & 0x3ff;
    let flags = 0;
    const after = ramHash(machine.memory);
    if (before[0] !== after[0] || before[1] !== after[1]) {
      // the script changed memory at the frame boundary: keep the bytes
      flags |= 1;
      const diff = [];
      for (const [base, old] of [[0x02000000, snapRam], [0x03000000, snapIw]]) {
        const now = new Uint8Array(machine.memory.buffer, base, old.length);
        for (let i = 0; i < old.length; i++) if (old[i] !== now[i]) diff.push([base + i, now[i]]);
      }
      edits.push({ frame: n, bytes: diff });
    }
    const [lo, hi] = before;
    const row = Buffer.alloc(32);
    row.writeUInt32LE(n, 0); row.writeUInt32LE(keys, 4); row.writeUInt32LE(solar, 8); row.writeUInt32LE(flags, 12);
    row.writeUInt32LE(lo, 16); row.writeUInt32LE(hi, 20); row.writeUInt32LE(total, 24); row.writeUInt32LE(0, 28);
    fs.writeSync(framesFd, row);
    if (n % every === 0 || flags) {
      fs.writeFileSync(path.join(out, 'snap', String(n).padStart(8, '0') + '.gz'),
        encodeSnapshot(snapshotBuffers(machine), machine.stackPointer()));
    }
    if (screens && n % screens === 0)
      fs.writeFileSync(path.join(out, 'screens', String(n).padStart(8, '0') + '.rgb'), Buffer.from(new Uint8Array(machine.memory.buffer, ptr, 240 * 160 * 2)));
    const done = (task && task.done) || (maxFrames && n + 1 >= maxFrames) || (!code && !maxFrames);
    if (done) {
      if (task && task.error) process.stderr.write('script error: ' + (task.error.stack || task.error) + '\n');
      meta.frames = n + 1;
      throw new Stop();
    }
    return keys;
  };
  machine = await boot({ save: args.save, time: meta.time, solar: meta.solar, onFrame, onFlush: () => flush() });
  setMode(machine, MODE_REC);
  try {
    machine.ex.port_game_run(machine.saveSize);
  } catch (e) {
    if (!(e instanceof Stop)) throw e;
  }
  fs.closeSync(framesFd);
  fs.closeSync(writesFd);
  meta.writeWords = writeWords;
  meta.seconds = (Date.now() - started) / 1000;
  meta.heapBase = machine.heapBase;
  fs.writeFileSync(path.join(out, 'edits.json'), JSON.stringify(edits));
  fs.writeFileSync(path.join(out, 'meta.json'), JSON.stringify(meta, null, 1));
  process.stdout.write(JSON.stringify({ session: out, frames: meta.frames, writeWords, seconds: meta.seconds }) + '\n');
}

// ---------------------------------------------------------------------------
// serve: replay with checks, tracing and perturbation, driven over stdin.
function readFrames(dir) {
  const buf = fs.readFileSync(path.join(dir, 'frames.bin'));
  const rows = [];
  for (let o = 0; o + 32 <= buf.length; o += 32)
    rows.push({ frame: buf.readUInt32LE(o), keys: buf.readUInt32LE(o + 4), solar: buf.readUInt32LE(o + 8),
      flags: buf.readUInt32LE(o + 12), lo: buf.readUInt32LE(o + 16), hi: buf.readUInt32LE(o + 20) });
  return rows;
}

function lineReader() {
  let buf = '';
  const chunk = Buffer.alloc(65536);
  return () => {
    for (;;) {
      const i = buf.indexOf('\n');
      if (i >= 0) { const line = buf.slice(0, i); buf = buf.slice(i + 1); return line; }
      let n;
      try { n = fs.readSync(0, chunk, 0, chunk.length, null); } catch (e) { if (e.code === 'EAGAIN') continue; throw e; }
      if (!n) return null;
      buf += chunk.toString('utf8', 0, n);
    }
  };
}

async function serve(args) {
  const dir = path.resolve(args._[1]);
  const meta = JSON.parse(fs.readFileSync(path.join(dir, 'meta.json'), 'utf8'));
  const rows = readFrames(dir);
  const edits = new Map(JSON.parse(fs.readFileSync(path.join(dir, 'edits.json'), 'utf8')).map((e) => [e.frame, e.bytes]));
  const snaps = fs.readdirSync(path.join(dir, 'snap')).map((f) => Number(f.slice(0, 8))).sort((a, b) => a - b);
  const read = lineReader();
  const reply = (o) => process.stdout.write(JSON.stringify(o) + '\n');
  let machine, current = -1, lastPixels = 0;
  let plan = null;          // {left, reply}: frames still to run before answering
  let override = { solar: null, keys: null }, perturbed = false, divergence = null;
  let traceDir = null, traceBufs = [], tracer = null;
  const flush = () => drain(machine, (b) => { if (traceDir) traceBufs.push(b); });

  const keysAfter = (n) => {
    if (!rows[n]) throw new Error('frame ' + n + ' is past the end of the recording');
    return override.keys ?? rows[n].keys;
  };
  function applyBoundary(n) {
    // what the recording did at the end of frame n: the script's edits, the next frame's light
    const e = edits.get(n);
    if (e) { const b = new Uint8Array(machine.memory.buffer); for (const [a, v] of e) b[a] = v; }
    machine.ex.port_game_set_solar(override.solar ?? (rows[n] ? rows[n].solar : meta.solar));
  }
  function check(n) {
    const [lo, hi] = ramHash(machine.memory);
    const r = rows[n];
    if (!r || (r.lo === lo && r.hi === hi)) return;
    if (!perturbed) throw new Error(`replay drifted at frame ${n}: RAM hash differs from the recording`);
    if (divergence === null) divergence = n;
  }
  function endTrace(n) {
    if (!traceDir) return;
    flush();
    const rows = new Rows(5);
    tracer(words(traceBufs), rows);
    fs.writeFileSync(path.join(traceDir, 'f' + String(n).padStart(8, '0') + '.bin'), rows.bytes());
    traceBufs = [];
  }
  function status() { return { frame: current, perturbed, divergence }; }
  function go(left, answer, trace) {
    // run frames from the current boundary: left more boundaries after the next one
    if (trace) {
      traceDir = path.resolve(trace);
      fs.mkdirSync(traceDir, { recursive: true });
      traceBufs = [];
      tracer = makeTracer();
      setMode(machine, MODE_TRACE);
    } else setMode(machine, 0);
    plan = { left, answer };
    applyBoundary(current);
    return keysAfter(current);
  }

  function onFrame(ptr, n) {
    lastPixels = ptr;
    if (current >= 0) {
      endTrace(n);
      try { check(n); } catch (e) {
        current = n; plan = null; traceDir = null; setMode(machine, 0);
        reply({ error: String(e.message) });
        return commands();
      }
    }
    current = n;
    if (plan && plan.left > 0) {
      plan.left--;
      applyBoundary(n);
      return keysAfter(n);
    }
    if (plan) {
      const p = plan; plan = null;
      if (traceDir) { traceDir = null; setMode(machine, 0); }
      reply({ ...p.answer, ...status() });
    }
    return commands();
  }

  function commands() {
    for (;;) {
      const line = read();
      if (line === null) throw new Stop();
      if (!line.trim()) continue;
      let cmd;
      try { cmd = JSON.parse(line); } catch (e) { reply({ error: 'bad json' }); continue; }
      try {
        switch (cmd.op) {
          case 'quit': reply({ ok: true }); throw new Stop();
          case 'seek': {
            const want = Number(cmd.frame);
            let s = -1;
            for (const x of snaps) if (x <= want) s = x;
            if (s < 0) throw new Error('no snapshot at or before frame ' + want);
            if (!(current >= s && current <= want && !perturbed)) {
              restore(machine, decodeSnapshot(fs.readFileSync(path.join(dir, 'snap', String(s).padStart(8, '0') + '.gz'))));
              current = s;
              perturbed = false; divergence = null; override = { solar: null, keys: null };
            }
            if (current === want) { reply({ seek: want, ...status() }); continue; }
            return go(want - current - 1, { seek: want });
          }
          case 'run': {
            const frames = Math.max(1, Number(cmd.frames || 1));
            return go(frames - 1, { ran: frames }, cmd.trace);
          }
          case 'solar': override.solar = cmd.level === null ? null : Number(cmd.level); perturbed = true; reply({ ok: true }); continue;
          case 'keys': override.keys = cmd.mask === null ? null : Number(cmd.mask); perturbed = true; reply({ ok: true }); continue;
          case 'poke': {
            const dv = new DataView(machine.memory.buffer);
            if (cmd.size === 1) dv.setUint8(cmd.addr, cmd.value);
            else if (cmd.size === 2) dv.setUint16(cmd.addr, cmd.value, true);
            else dv.setUint32(cmd.addr, cmd.value >>> 0, true);
            perturbed = true; reply({ ok: true }); continue;
          }
          case 'read': reply({ hex: Buffer.from(new Uint8Array(machine.memory.buffer, cmd.addr, cmd.len)).toString('hex') }); continue;
          case 'ram':
            fs.writeFileSync(cmd.file, Buffer.concat([Buffer.from(new Uint8Array(machine.memory.buffer, 0x02000000, 0x40000)),
              Buffer.from(new Uint8Array(machine.memory.buffer, 0x03000000, 0x8000))]));
            reply({ ok: true }); continue;
          case 'shot': png(new Uint16Array(machine.memory.buffer, lastPixels, 240 * 160), cmd.file); reply({ ok: true }); continue;
          case 'status': reply({ ...status(), hash: ramHash(machine.memory), frames: rows.length }); continue;
          default: reply({ error: 'unknown op ' + cmd.op });
        }
      } catch (e) {
        if (e instanceof Stop) throw e;
        reply({ error: String(e.message || e) });
      }
    }
  }

  machine = await boot({ save: meta.save ? path.join(ROOT, meta.save) : null, time: meta.time, solar: meta.solar,
    onFrame, onFlush: () => flush() });
  setMode(machine, 0);
  reply({ ready: true, frames: rows.length });
  try {
    machine.ex.port_game_run(machine.saveSize);
  } catch (e) {
    if (!(e instanceof Stop)) { reply({ fatal: String(e.stack || e) }); process.exitCode = 1; }
  }
}

const args = parseArgs(process.argv.slice(2));
if (args._[0] === 'record') await record(args);
else if (args._[0] === 'serve') await serve(args);
else { process.stderr.write('usage: runner.mjs record OUT [...] | serve SESSION\n'); process.exit(2); }
