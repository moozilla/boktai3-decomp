// Headless in-process sessions of the full-game module, for programs that play the
// game (tools/autopilot/): no daemon, no HTTP, no pacing, no screen posts.
//
//   import { openSession } from './session.mjs';
//   const s = await openSession({ save: 'build/saves/0002.sav' });
//   const result = s.run(function* (g, s) {        // g: gameapi.mjs (typed $, hooks, press)
//     yield g.press('start', 6);
//     const snap = s.snapshot();                   // cheap: the live parts of memory
//     yield 120;
//     s.restore(snap);                             // back to that frame, movie truncated
//     return g.$.gPlayers[0];
//   });
//   s.movie                                         // key mask of every frame played
//   openSession({ movie })                          // start from a recorded movie (replayed by the first run)
//
// The PPU is off unless opts.render (or s.setRender(true) before a frame you want to
// see): the game never reads the screen, and skipping it is most of a frame's cost.
//
// The game runs as native wasm calls; the script runs inside the per-frame import,
// at the VBlank wait. A snapshot is the shadow stack in use, the module's data and the
// GBA regions; restoring it is only valid at a frame boundary with the same wasm stack
// pointer (checked), which holds at the game's main-loop VBlank wait. `run` returns
// when the script finishes (the game's native stack is unwound; the next `run` boots
// again and replays the movie, so it stays deterministic).
import fs from 'node:fs';
import path from 'node:path';
import * as zlibModule from 'node:zlib';
import { createGame, keyMask } from './gameapi.mjs';

const ROOT = path.resolve(path.dirname(new URL(import.meta.url).pathname), '../..');
const REGIONS = [[0x02000000, 0x40000], [0x03000000, 0x8000], [0x04000000, 0x400], [0x05000000, 0x400],
  [0x06000000, 0x18000], [0x07000000, 0x400]];
const STACK_TOP = 4194304; // -z stack-size, --stack-first (port/build_game.py)

class Stop extends Error {}

export async function openSession(opts = {}) {
  const wasmPath = opts.wasm || path.join(ROOT, 'build/port/game/game.wasm');
  const rom = fs.readFileSync(opts.rom || path.join(ROOT, 'baserom.gba'));
  const typesPath = opts.typesPath || path.join(ROOT, 'build/port/agent/types.json');
  const types = fs.existsSync(typesPath) ? JSON.parse(fs.readFileSync(typesPath, 'utf8')) : { types: {}, globals: {}, functions: {}, data: {} };
  const libDir = opts.lib === undefined ? [path.join(ROOT, 'port/agent/lib')] : [].concat(opts.lib || []);
  const save = opts.save ? fs.readFileSync(opts.save) : null;
  const time = Math.floor(Date.parse(opts.time || '2025-11-13T12:00:00Z') / 1000);
  let solar = opts.solar ?? 232;
  let render = !!opts.render;
  const out = opts.print || ((s) => process.stderr.write(s + '\n'));

  let ex = null, memory = null, g = null, frameNo = 0, lastPixels = 0;
  let script = null, replay = null, stopped = null, pending = null;
  const movie = (opts.movie || []).slice();  // movie[n] = keys returned for frame n; opts.movie replays first
  const solarLog = [];   // [frame, value] changes
  const session = {};

  const imports = { env: {
    frame(ptr, n) {
      frameNo = n;
      lastPixels = ptr;
      let keys;
      if (replay && n < replay.length) {
        keys = replay[n] | 0;
      } else {
        replay = null;
        if (!script) {
          if (!pending) throw new Stop();
          const gen = pending;
          pending = null;
          script = g.spawn(() => gen(g, session), { name: 'autopilot' });
        }
        if (!script.done) keys = g.frame(n);
        if (script.done) { stopped = script; throw new Stop(); }
      }
      // frameNo, not n: a restore inside this call moved the game back to snap.frame,
      // and these keys are what that frame runs with
      movie.length = frameNo + 1;
      movie[frameNo] = keys;
      return keys;
    },
    audio() {},
    hook(kind, address) { if (g) g.dispatch(kind, address); },
    log(msg, value) { out(`[game] ${cstr(msg)} 0x${(value >>> 0).toString(16)}`); },
    fatal(msg, value) { out(`[fatal] ${cstr(msg)} 0x${(value >>> 0).toString(16)}`); },
  } };
  const cstr = (p) => { const b = new Uint8Array(memory.buffer, p); let e = 0; while (b[e]) e++; return Buffer.from(b.slice(0, e)).toString(); };
  const module = await WebAssembly.compile(fs.readFileSync(wasmPath));
  const libSources = {};
  for (const dir of libDir) {
    if (!fs.existsSync(dir)) continue;
    for (const f of fs.readdirSync(dir).sort()) if (f.endsWith('.js')) libSources[path.join(dir, f)] = fs.readFileSync(path.join(dir, f), 'utf8');
  }

  function boot() {
    const instance = new WebAssembly.Instance(module, imports);
    ex = instance.exports;
    memory = ex.memory;
    if (ex.port_memory_init() !== 0) throw new Error('memory init failed');
    new Uint8Array(memory.buffer, 0x08000000, rom.length).set(rom);
    if (ex.port_load_rom(0x08000000, rom.length) !== 0) throw new Error('ROM load failed');
    ex.port_game_set_time(time);
    ex.port_game_set_solar(solar);
    if (ex.port_video_set_enabled) ex.port_video_set_enabled(render ? 1 : 0);
    g = createGame({ memory, exports: ex, types, print: out });
    g.loadLib(libSources);
    if (save) new Uint8Array(memory.buffer, ex.port_game_save_stage(), save.length).set(save);
    return save ? save.length : 0;
  }

  function dataRange() {
    const base = ex.__heap_base ? ex.__heap_base.value : 0;
    return [STACK_TOP, base || STACK_TOP];
  }
  function sp() { return ex.__stack_pointer ? ex.__stack_pointer.value : -1; }

  Object.defineProperties(session, {
    g: { get: () => g }, frame: { get: () => frameNo }, movie: { get: () => movie },
    exports: { get: () => ex }, memory: { get: () => memory }, solarLog: { get: () => solarLog },
  });
  Object.assign(session, {
    types,
    // Run gen(g, session) as a script from where the last run stopped. The first run
    // boots; a later one boots again and replays the movie up to the same frame (the
    // previous run's native stack was unwound), so the script starts where it left off.
    run(gen) {
      stopped = null;
      const saveSize = boot();
      replay = movie.slice();
      movie.length = 0;
      script = null;
      pending = gen;
      try { ex.port_game_run(saveSize); } catch (e) { if (!(e instanceof Stop)) throw e; }
      const t = stopped;
      script = null;
      pending = null;
      if (!t || !t.done) throw new Error('the game stopped before the script finished');
      if (t.error) throw t.error;
      return t.result;
    },
    snapshot() {
      const [d0, d1] = dataRange();
      const s = sp();
      const parts = [[s, new Uint8Array(memory.buffer.slice(s, STACK_TOP))], [d0, new Uint8Array(memory.buffer.slice(d0, d1))]];
      for (const [a, n] of REGIONS) parts.push([a, new Uint8Array(memory.buffer.slice(a, a + n))]);
      // the movie itself, not just its length: another branch may have overwritten it
      return { frame: frameNo, sp: s, parts, movie: movie.length, keys: movie.slice(), solar };
    },
    restore(snap) {
      if (snap.sp !== sp()) throw new Error(`restore at a different stack depth (sp ${sp()} vs ${snap.sp})`);
      const m = new Uint8Array(memory.buffer);
      for (const [a, bytes] of snap.parts) m.set(bytes, a);
      movie.length = 0;
      for (let i = 0; i < snap.movie; i++) movie.push(snap.keys[i]);
      frameNo = snap.frame;
      if (snap.solar !== solar) session.setSolar(snap.solar);
      g.refresh();  // hooks set since the snapshot
    },
    setRender(on) { render = !!on; if (ex && ex.port_video_set_enabled) ex.port_video_set_enabled(render ? 1 : 0); },
    setSolar(v) { solar = v; ex.port_game_set_solar(v); solarLog.push([frameNo, v]); },
    pixels() { return new Uint16Array(memory.buffer, lastPixels, 240 * 160).slice(); },
    keyMask,
  });
  return session;
}

// PNG of an RGB555 frame.
export function png(rgb555) {
  const w = 240, h = 160, raw = Buffer.alloc((w * 3 + 1) * h);
  for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) {
    const c = rgb555[y * w + x], o = y * (w * 3 + 1) + 1 + x * 3, r = c & 31, gg = (c >> 5) & 31, b = (c >> 10) & 31;
    raw[o] = (r << 3) | (r >> 2); raw[o + 1] = (gg << 3) | (gg >> 2); raw[o + 2] = (b << 3) | (b >> 2);
  }
  const table = png.t || (png.t = Array.from({ length: 256 }, (_, n) => { let c = n; for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1; return c >>> 0; }));
  const crc = (b) => { let c = 0xffffffff; for (const x of b) c = table[(c ^ x) & 255] ^ (c >>> 8); return (c ^ 0xffffffff) >>> 0; };
  const chunk = (t, d) => { const l = Buffer.alloc(4); l.writeUInt32BE(d.length); const td = Buffer.concat([Buffer.from(t), d]); const c = Buffer.alloc(4); c.writeUInt32BE(crc(td)); return Buffer.concat([l, td, c]); };
  const ih = Buffer.alloc(13); ih.writeUInt32BE(w, 0); ih.writeUInt32BE(h, 4); ih[8] = 8; ih[9] = 2;
  return Buffer.concat([Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]), chunk('IHDR', ih), chunk('IDAT', zlibModule.deflateSync(raw, { level: 1 })), chunk('IEND', Buffer.alloc(0))]);
}
