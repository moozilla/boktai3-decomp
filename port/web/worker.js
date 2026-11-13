// Runs the full-game module (build/port/game/game.wasm). AgbMain never
// returns, so the game owns this worker; at each VBlank the frame import
// presents the picture, sleeps to keep 59.73 fps and reads the shared input.
//
// Studio (port/agent/gameapi.mjs): the page's console, watches, task list,
// subtitles and sprite outlines run here, between frames. The page writes
// requests into a shared mailbox (the worker never returns to its event loop,
// so postMessage cannot reach it); replies go back with postMessage.
//
// Shared control words (Int32Array over `control`):
//   0 keys (KEYINPUT order, 1 = pressed)   1 fast-forward   2 pause
//   3 solar level 0 (sun) .. 255 (dark)    4 sleep slot (always 0)
//   5 keys pressed since the last frame (a tap shorter than a frame still counts)
// Audio ring (`audio`): Int32 [write, read, rate], then Float32 stereo data.
const FRAME_MS = 1000 / (16777216 / 280896);
let ex, memory, control, audioHead, audioData, audioFrames;
let started = 0, solar = -1, lastSave = 0;
let g = null, mailbox = null, mailBytes = null;
const studio = { watches: [], watchEvery: 10, inspect: false, subtitles: false, subtitleHooks: [] };

function cstr(ptr) {
  const b = new Uint8Array(memory.buffer, ptr);
  let e = 0;
  while (b[e]) e++;
  return new TextDecoder().decode(b.slice(0, e));
}

function setClock() {
  const local = Math.floor(Date.now() / 1000) - new Date().getTimezoneOffset() * 60;
  ex.port_game_set_time(local >>> 0);
}

const imports = { env: {
  frame(ptr, n) {
    let scripted = 0;
    if (g) {
      scripted = g.frame(n);
      serveMailbox();
      studioFrame(n);
    }
    const src = new Uint16Array(memory.buffer, ptr, 240 * 160);
    const rgba = new Uint32Array(240 * 160);
    for (let i = 0; i < rgba.length; i++) {
      const c = src[i];
      const r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
      rgba[i] = 0xff000000 | (((b << 3) | (b >> 2)) << 16) | (((g << 3) | (g >> 2)) << 8) | ((r << 3) | (r >> 2));
    }
    postMessage({ type: 'frame', n, pixels: rgba.buffer }, [rgba.buffer]);
    if (n % 60 === 0) setClock();
    const level = Atomics.load(control, 3);
    if (level !== solar) { solar = level; ex.port_game_set_solar(level); }
    if (ex.port_game_save_dirty() && n - lastSave > 30) {
      lastSave = n;
      const data = new Uint8Array(memory.buffer, ex.port_game_save_data(), ex.port_game_save_size()).slice();
      postMessage({ type: 'save', data }, [data.buffer]);
    }
    while (Atomics.load(control, 2)) Atomics.wait(control, 4, 0, 50);
    const now = performance.now();
    if (Atomics.load(control, 1)) {
      started = now - n * FRAME_MS;
    } else {
      const due = started + (n + 1) * FRAME_MS;
      if (due - now > 100 || now - due > 100) started = now - n * FRAME_MS; // resync after stalls
      else if (due > now) Atomics.wait(control, 4, 0, due - now);
    }
    return Atomics.load(control, 0) | Atomics.exchange(control, 5, 0) | scripted;
  },
  audio(ptr, frames, rate) {
    if (!audioData || Atomics.load(control, 1)) return;
    const pcm = new Int16Array(memory.buffer, ptr, frames * 2);
    let write = Atomics.load(audioHead, 0);
    const read = Atomics.load(audioHead, 1);
    Atomics.store(audioHead, 2, rate);
    for (let i = 0; i < frames; i++) {
      if (((write + 1) % audioFrames) === read) break; // full: drop
      audioData[write * 2] = pcm[i * 2] / 32768;
      audioData[write * 2 + 1] = pcm[i * 2 + 1] / 32768;
      write = (write + 1) % audioFrames;
    }
    Atomics.store(audioHead, 0, write);
  },
  hook(kind, address) { if (g) g.dispatch(kind, address); },
  log(msg, value) { postMessage({ type: 'log', text: `${cstr(msg)} 0x${(value >>> 0).toString(16)}` }); },
  fatal(msg, value) { postMessage({ type: 'fatal', text: `${cstr(msg)} 0x${(value >>> 0).toString(16)}` }); },
} };

onmessage = async (event) => {
  const { wasm, rom, save, controlBuffer, audioBuffer, mailboxBuffer } = event.data;
  control = new Int32Array(controlBuffer);
  if (mailboxBuffer) { mailbox = new Int32Array(mailboxBuffer, 0, 2); mailBytes = new Uint8Array(mailboxBuffer, 8); }
  if (audioBuffer) {
    audioHead = new Int32Array(audioBuffer, 0, 4);
    audioData = new Float32Array(audioBuffer, 16);
    audioFrames = audioData.length / 2;
  }
  try {
    const { instance } = await WebAssembly.instantiate(wasm, imports);
    ex = instance.exports;
    memory = ex.memory;
    if (ex.port_memory_init() !== 0) throw new Error('memory init failed');
    new Uint8Array(memory.buffer, 0x08000000, rom.byteLength).set(new Uint8Array(rom));
    if (ex.port_load_rom(0x08000000, rom.byteLength) !== 0) throw new Error('ROM load failed');
    let saveSize = 0;
    if (save && save.byteLength === ex.port_game_save_size()) {
      new Uint8Array(memory.buffer, ex.port_game_save_stage(), save.byteLength).set(new Uint8Array(save));
      saveSize = save.byteLength;
    }
    await startStudio();
    setClock();
    started = performance.now();
    postMessage({ type: 'started' });
    const result = ex.port_game_run(saveSize);
    postMessage({ type: 'fatal', text: `port_game_run returned ${result}` });
  } catch (e) {
    postMessage({ type: 'fatal', text: String(e && e.stack || e) });
  }
};

// ---- studio ---------------------------------------------------------------
async function startStudio() {
  if (!mailbox) return;
  let types = null, lib = {}, createGame = null;
  try {
    ({ createGame } = await import('./agent/gameapi.mjs'));
    const r = await fetch('types.json');
    if (r.ok) types = await r.json();
    const l = await fetch('agent/lib.json');
    if (l.ok) lib = await l.json();
  } catch (e) { /* hosted copy: no studio data */ }
  if (!types || !createGame) { postMessage({ type: 'studio', ready: false }); return; }
  g = createGame({ memory, exports: ex, types, print: (text) => postMessage({ type: 'print', text }) });
  g.loadLib(lib);
  postMessage({ type: 'studio', ready: true, globals: Object.keys(types.globals).length, functions: Object.keys(types.functions).length, lib: Object.keys(g.lib) });
}

function serveMailbox() {
  if (!Atomics.load(mailbox, 0)) return;
  const n = Atomics.load(mailbox, 1);
  let req;
  try { req = JSON.parse(new TextDecoder().decode(mailBytes.slice(0, n))); } catch (e) { req = null; }
  Atomics.store(mailbox, 0, 0);
  if (req) handle(req);
}

function reply(id, body) { postMessage({ type: 'reply', id, ...body }); }

function handle(req) {
  try {
    switch (req.op) {
      case 'js': {
        const t = g.spawn(g.compile(req.code), { name: 'console' });
        const answer = (x) => reply(req.id, x.error ? { error: String(x.error && x.error.stack || x.error) } : { value: g.format(x.result, req.depth ?? 1) });
        if (t.done) answer(t); else { t.onDone = answer; reply(req.id, { pending: true }); }
        return;
      }
      case 'watch':
        studio.watches = (req.exprs || []).map((e) => { try { return { e, f: g.compile(e) }; } catch (err) { return { e, err: String(err) }; } });
        return reply(req.id, { ok: true });
      case 'tasks':
        return reply(req.id, { tasks: g.lib.tasks ? g.lib.tasks() : [] });
      case 'inspect': setInspect(!!req.on); return reply(req.id, { ok: true });
      case 'subtitles': setSubtitles(!!req.on); return reply(req.id, { ok: true });
      case 'stop': g.off(); g.tasks.splice(0); setInspect(studio.inspect); setSubtitles(studio.subtitles); return reply(req.id, { ok: true });
      default: return reply(req.id, { error: 'unknown op ' + req.op });
    }
  } catch (e) {
    reply(req.id, { error: String(e && e.stack || e) });
  }
}

function studioFrame(n) {
  if (studio.watches.length && n % studio.watchEvery === 0) {
    const values = studio.watches.map((w) => {
      if (w.err) return w.err;
      try { const it = w.f(); const r = it.next(); return g.format(r.value, 0); } catch (e) { return 'error: ' + (e && e.message || e); }
    });
    postMessage({ type: 'watch', values });
  }
  if (studio.inspect && n % 2 === 0) postMessage({ type: 'boxes', ...inspectFrame() });
}

const OBJ_SIZE = [[[8, 8], [16, 16], [32, 32], [64, 64]], [[16, 8], [32, 8], [32, 16], [64, 32]], [[8, 16], [8, 32], [16, 32], [32, 64]]];
function oamBox(word) {
  const a0 = word & 0xffff, a1 = word >>> 16;
  if ((a0 & 0x300) === 0x200) return null;
  let [w, h] = (OBJ_SIZE[a0 >> 14] || OBJ_SIZE[0])[a1 >> 14];
  if (a0 & 0x200) { w *= 2; h *= 2; }
  let y = a0 & 0xff, x = a1 & 0x1ff;
  if (y >= 160) y -= 256;
  if (x >= 240) x -= 512;
  return [x, y, w, h];
}

// Inspect: projected sprites (lib drawables(): screen box, owner task) and
// every OAM entry the PPU shows this frame.
function setInspect(on) { studio.inspect = on; }
function inspectFrame() {
  let boxes = [];
  try { boxes = g.lib.drawables ? g.lib.drawables().filter((d) => !d.hidden) : []; } catch (e) { boxes = []; }
  const oam = [];
  for (let i = 0; i < 128; i++) { const b = oamBox(g.u32(0x07000000 + 8 * i)); if (b) oam.push(b); }
  return { boxes, oam };
}

// Subtitles: the string the message window shows, and the window's bounds (tiles).
function setSubtitles(on) {
  for (const id of studio.subtitleHooks) g.off(id);
  studio.subtitleHooks = [];
  studio.subtitles = on;
  if (!on) return;
  const has = (n) => n in g.types.functions;
  if (has('Text_LookupString')) studio.subtitleHooks.push(g.on('Text_LookupString', (a) => postMessage({ type: 'text', id: a[0], frame: g.frameNo })));
  if (has('MessageWindow_Clear')) studio.subtitleHooks.push(g.on('MessageWindow_Clear', () => postMessage({ type: 'text-clear', frame: g.frameNo })));
  if (has('MessageWindow_SetBounds')) studio.subtitleHooks.push(g.on('MessageWindow_SetBounds', (a) => postMessage({ type: 'text-bounds', bounds: a.map((x) => x | 0) })));
}
