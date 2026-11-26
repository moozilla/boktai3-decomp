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
//   openSession({ movie, onBoot: (g) => g.on(...) }) // hooks installed at boot see the replay too
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
import { fileURLToPath } from 'node:url';
import * as zlibModule from 'node:zlib';
import { createGame, keyMask } from './gameapi.mjs';
import { runGame } from '../web/run-game.mjs';

const ROOT = fileURLToPath(new URL('../..', import.meta.url));
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
  const solar0 = opts.solar ?? 232;
  let solar = solar0;
  // solar sensor changes by frame ([frame, value]): opts.solarLog replays an earlier run's;
  // setSolar adds to it. Applied at the frame boundary, in replays too.
  const solarAt = new Map((opts.solarLog || []).map(([f, v]) => [f, v]));
  let render = !!opts.render;
  const out = opts.print || ((s) => process.stderr.write(s + '\n'));

  let ex = null, memory = null, g = null, frameNo = 0, lastPixels = 0;
  let script = null, replay = null, stopped = null, pending = null, stepping = false;
  const movie = (opts.movie || []).slice();  // movie[n] = keys returned for frame n; opts.movie replays first
  const session = {};

  const imports = { env: {
    frame(ptr, n) {
      frameNo = n;
      lastPixels = ptr;
      if (opts.onFrame) opts.onFrame(n);
      if (solarAt.has(n) && solarAt.get(n) !== solar) { solar = solarAt.get(n); ex.port_game_set_solar(solar); }
      let keys;
      if (stepping) {
        // Calls made by a scenario can themselves wait for VBlank. The running
        // generator cannot be resumed from inside its own call into the game.
        keys = g.currentKeys();
      } else if (replay && n < replay.length) {
        keys = replay[n] | 0;
      } else {
        replay = null;
        stepping = true;
        try {
          if (!script) {
            if (!pending) throw new Stop();
            const gen = pending;
            pending = null;
            script = g.spawn(() => gen(g, session), { name: opts.name || 'autopilot' });
          }
          if (!script.done) keys = g.frame(frameNo);
          if (script.done) { stopped = script; throw new Stop(); }
        } finally {
          stepping = false;
        }
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
    solar = solarAt.has(0) ? solarAt.get(0) : solar0;
    ex.port_game_set_solar(solar);
    if (ex.port_video_set_enabled) ex.port_video_set_enabled(render ? 1 : 0);
    g = createGame({ memory, exports: ex, types, print: out, onWrite: opts.onWrite });
    g.loadLib(libSources);
    if (opts.coverage) session.setCoverage(true);
    if (save) new Uint8Array(memory.buffer, ex.port_game_save_stage(), save.length).set(save);
    if (opts.onBoot) opts.onBoot(g, session);  // hooks that also see the replayed movie
    return save ? save.length : 0;
  }

  function dataRange() {
    const base = ex.__heap_base ? ex.__heap_base.value : 0;
    return [STACK_TOP, base || STACK_TOP];
  }
  function sp() { return ex.__stack_pointer ? ex.__stack_pointer.value : -1; }

  Object.defineProperties(session, {
    g: { get: () => g }, frame: { get: () => frameNo }, movie: { get: () => movie },
    exports: { get: () => ex }, memory: { get: () => memory },
    solar: { get: () => solar },
    solarLog: { get: () => [...solarAt].sort((a, b) => a[0] - b[0]).filter(([f]) => f < movie.length) },
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
      try { runGame(ex, saveSize); } catch (e) { if (!(e instanceof Stop)) throw e; }
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
      return { frame: frameNo, sp: s, parts, movie: movie.length, keys: movie.slice(), solar, solarAt: new Map(solarAt) };
    },
    restore(snap) {
      if (snap.sp !== sp()) throw new Error(`restore at a different stack depth (sp ${sp()} vs ${snap.sp})`);
      const m = new Uint8Array(memory.buffer);
      for (const [a, bytes] of snap.parts) m.set(bytes, a);
      movie.length = 0;
      for (let i = 0; i < snap.movie; i++) movie.push(snap.keys[i]);
      frameNo = snap.frame;
      solar = snap.solar;
      solarAt.clear();
      for (const [f, v] of snap.solarAt) solarAt.set(f, v);
      g.refresh();  // hooks set since the snapshot
    },
    setRender(on) { render = !!on; if (ex && ex.port_video_set_enabled) ex.port_video_set_enabled(render ? 1 : 0); },
    setCoverage(on, { clear = false } = {}) {
      if (clear) session.coverageMap().fill(0);
      // coverage is word 16 of struct HookState in port/game/hooks.c.
      new DataView(memory.buffer).setUint32(ex.port_hook_state() + 16 * 4, Number(on), true);
    },
    coverageMap() { return new Uint8Array(memory.buffer, ex.port_trace_map(), 0x800000); },
    setSolar(v) {
      if (!Number.isInteger(v) || v < 0 || v > 255) throw new RangeError('solar threshold must be an integer from 0 to 255');
      if (v === solar) return;
      solar = v;
      ex.port_game_set_solar(v);
      solarAt.set(frameNo, v);
    },
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
