// Runs the full-game module (build/port/game/game.wasm). AgbMain never
// returns, so the game owns this worker; at each VBlank the frame import
// presents the picture, sleeps to keep 59.73 fps and reads the shared input.
//
// Shared control words (Int32Array over `control`):
//   0 keys (KEYINPUT order, 1 = pressed)   1 fast-forward   2 pause
//   3 solar level 0 (sun) .. 255 (dark)    4 sleep slot (always 0)
//   5 keys pressed since the last frame (a tap shorter than a frame still counts)
// Audio ring (`audio`): Int32 [write, read, rate], then Float32 stereo data.
const FRAME_MS = 1000 / (16777216 / 280896);
let ex, memory, control, audioHead, audioData, audioFrames;
let started = 0, solar = -1, lastSave = 0;

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
    return Atomics.load(control, 0) | Atomics.exchange(control, 5, 0);
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
  log(msg, value) { postMessage({ type: 'log', text: `${cstr(msg)} 0x${(value >>> 0).toString(16)}` }); },
  fatal(msg, value) { postMessage({ type: 'fatal', text: `${cstr(msg)} 0x${(value >>> 0).toString(16)}` }); },
} };

onmessage = async (event) => {
  const { wasm, rom, save, controlBuffer, audioBuffer } = event.data;
  control = new Int32Array(controlBuffer);
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
    setClock();
    started = performance.now();
    postMessage({ type: 'started' });
    const result = ex.port_game_run(saveSize);
    postMessage({ type: 'fatal', text: `port_game_run returned ${result}` });
  } catch (e) {
    postMessage({ type: 'fatal', text: String(e && e.stack || e) });
  }
};
