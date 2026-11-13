#!/usr/bin/env node
// Agent harness daemon: runs the full-game module and serves commands over
// HTTP on 127.0.0.1 (port/agent/play.py is the client; README.md lists them).
//
// The game runs in a worker thread. It stops at frame boundaries (after a
// `run`) or at a breakpoint (a function entry/exit, port/game/hooks.c) and
// serves commands there: read/write memory, call game functions, run C
// snippets, arm breakpoints and traces. The main thread only relays requests
// and keeps the latest screen for the viewer page (GET /).
//
//   node port/agent/daemon.mjs [--port 8766] [--save F.sav] [--time ISO] [--solar N]
import fs from 'node:fs';
import http from 'node:http';
import path from 'node:path';
import zlib from 'node:zlib';
import { Worker, isMainThread, parentPort, workerData, receiveMessageOnPort, MessageChannel } from 'node:worker_threads';

const ROOT = path.resolve(path.dirname(new URL(import.meta.url).pathname), '../..');
const STATE_DIR = path.join(ROOT, 'build/port/agent');

function png(rgba, w, h) {
  const raw = Buffer.alloc((w * 3 + 1) * h);
  for (let y = 0; y < h; y++) {
    raw[y * (w * 3 + 1)] = 0;
    for (let x = 0; x < w; x++) {
      const c = rgba[y * w + x], o = y * (w * 3 + 1) + 1 + x * 3;
      raw[o] = c & 255; raw[o + 1] = (c >> 8) & 255; raw[o + 2] = (c >> 16) & 255;
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
  return Buffer.concat([Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]), chunk('IHDR', ih),
    chunk('IDAT', zlib.deflateSync(raw, { level: 1 })), chunk('IEND', Buffer.alloc(0))]);
}

function rgba555(src) {
  const out = new Uint32Array(240 * 160);
  for (let i = 0; i < out.length; i++) {
    const c = src[i], r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
    out[i] = 0xff000000 | (((b << 3) | (b >> 2)) << 16) | (((g << 3) | (g >> 2)) << 8) | ((r << 3) | (r >> 2));
  }
  return out;
}

if (isMainThread) main(); else gameWorker();

// ---------------------------------------------------------------------------
// main thread: HTTP <-> worker
function main() {
  const args = Object.fromEntries(process.argv.slice(2).reduce((acc, a, i, all) => {
    if (a.startsWith('--')) acc.push([a.slice(2), all[i + 1] && !all[i + 1].startsWith('--') ? all[i + 1] : '1']);
    return acc;
  }, []));
  const port = Number(args.port || 8766);
  const ctl = new Int32Array(new SharedArrayBuffer(16));
  const { port1, port2 } = new MessageChannel();
  const worker = new Worker(new URL(import.meta.url), {
    workerData: { ctl, port: port2, wasm: args.wasm || path.join(ROOT, 'build/port/game/game.wasm'),
      rom: args.rom || path.join(ROOT, 'baserom.gba'), save: args.save || null,
      time: args.time || '2025-11-13T12:00:00Z', solar: Number(args.solar ?? 232) },
    transferList: [port2],
  });
  let screen = null, screenPng = null, nextId = 1;
  const pending = new Map();
  let lastStatus = {};
  worker.on('message', (m) => {
    if (m.type === 'screen') { screen = m.rgba; screenPng = null; return; }
    if (m.type === 'reply') {
      const p = pending.get(m.id);
      if (p) { pending.delete(m.id); p(m.result); }
      if (m.result && m.result.frame !== undefined) lastStatus = m.result;
    }
  });
  worker.on('error', (e) => { console.error('worker error', e); process.exit(1); });
  worker.on('exit', (code) => { console.error('worker exited', code); process.exit(1); });
  const send = (cmd) => new Promise((resolve) => {
    const id = nextId++;
    pending.set(id, resolve);
    port1.postMessage({ ...cmd, id });
    Atomics.store(ctl, 0, 1);
    Atomics.notify(ctl, 0);
  });

  const server = http.createServer(async (req, res) => {
    const url = new URL(req.url, 'http://x');
    try {
      if (req.method === 'POST' && url.pathname === '/cmd') {
        const chunks = [];
        for await (const c of req) chunks.push(c);
        const cmd = JSON.parse(Buffer.concat(chunks).toString() || '{}');
        if (cmd.op === 'interrupt') { Atomics.store(ctl, 1, 1); res.end('{"ok":true}'); return; }
        if (cmd.op === 'quit') { res.end('{"ok":true}'); setTimeout(() => process.exit(0), 50); return; }
        const result = await send(cmd);
        res.setHeader('Content-Type', 'application/json');
        res.end(JSON.stringify(result));
      } else if (url.pathname === '/screen.png') {
        if (!screen) { res.statusCode = 404; res.end(); return; }
        if (!screenPng) screenPng = png(new Uint32Array(screen), 240, 160);
        res.setHeader('Content-Type', 'image/png');
        res.setHeader('Cache-Control', 'no-store');
        res.end(screenPng);
      } else if (url.pathname === '/status.json') {
        res.setHeader('Content-Type', 'application/json');
        res.end(JSON.stringify(lastStatus));
      } else if (url.pathname === '/') {
        res.setHeader('Content-Type', 'text/html; charset=utf-8');
        res.end(VIEWER);
      } else { res.statusCode = 404; res.end(); }
    } catch (e) {
      res.statusCode = 500;
      res.end(JSON.stringify({ error: String(e && e.stack || e) }));
    }
  });
  server.listen(port, '127.0.0.1', () => {
    fs.mkdirSync(STATE_DIR, { recursive: true });
    fs.writeFileSync(path.join(STATE_DIR, 'daemon.json'), JSON.stringify({ port, pid: process.pid }));
    console.log(`agent daemon on http://127.0.0.1:${port}/`);
  });
}

const VIEWER = `<!doctype html><meta charset="utf-8"><title>Boktai 3 agent view</title>
<style>body{margin:0;background:#15171c;color:#e8e6e1;font:13px ui-monospace,monospace;padding:16px}
img{width:720px;max-width:100%;image-rendering:pixelated;display:block;background:#000;border-radius:4px}
#s{margin-top:8px;white-space:pre-wrap;color:#9a9890}</style>
<img id="i" alt="game screen"><div id="s"></div>
<script>
async function tick(){
  const i=new Image(); i.onload=()=>{document.getElementById('i').src=i.src}; i.src='/screen.png?'+Date.now();
  try{const s=await (await fetch('/status.json')).json(); document.getElementById('s').textContent=JSON.stringify(s,null,1)}catch(e){}
  setTimeout(tick,250)}
tick();
</script>`;

// ---------------------------------------------------------------------------
// worker: the game
function gameWorker() {
  const { ctl, port: commandPort, solar } = workerData;
  const romBytes = fs.readFileSync(workerData.rom);
  const wasmBytes = fs.readFileSync(workerData.wasm);
  const BUTTONS = { a: 0, b: 1, select: 2, start: 3, right: 4, left: 5, up: 6, down: 7, r: 8, l: 9 };
  class Restart extends Error {}

  let ex, mem, hookState, flags;
  let frameNo = 0, lastPixels = 0, lastScreenPost = 0;
  let serving = false;
  let run = null;           // {steps: [[mask, frames]], index, left, untilBreak, reply}
  let keys = 0;
  let stop = null;          // why we are serving: {reason, ...}
  let output = [];
  const breaks = new Map(); // address -> {enter, exit}
  const actions = new Map(); // address -> [cmd] one-shot, run at the next entry
  const observers = new Map(); // address -> [[arg, offset, length]...]: record each entry, keep going
  let observed = [];
  const OBSERVE_LIMIT = 200000;
  let log = [];             // commands that change the game, for checkpoints
  const checkpoints = new Map();
  let currentSave = workerData.save ? fs.readFileSync(workerData.save) : null;
  let replaying = false;
  let replayQueue = null;   // checkpoint commands still to replay
  let pendingReply = null;  // restart/restore request answered at the first stop

  const u8 = () => new Uint8Array(mem.buffer);
  const dv = () => new DataView(mem.buffer);
  const S = { active: 0, tracing: 1, suspend: 2, root: 3, inside: 4, count: 5, capacity: 6, overflow: 7,
    hit_kind: 8, hit_address: 9, args: 10, depth: 14, frame: 15 };
  const hs = (k, v) => {
    const a = hookState + 4 * S[k];
    if (v === undefined) return dv().getUint32(a, true);
    dv().setUint32(a, v >>> 0, true);
  };
  const flagIndex = (addr) => {
    if (addr >= 0x03000000 && addr < 0x0300110c) addr = addr - 0x03000000 + 0x08000350;
    return (addr - 0x08000000) >>> 1;
  };
  function refreshFlags() {
    const f = u8();
    for (const [addr, b] of breaks) f[flags + flagIndex(addr)] = (b.enter ? 1 : 0) | (b.exit ? 2 : 0);
    for (const addr of actions.keys()) f[flags + flagIndex(addr)] |= 1;
    for (const addr of observers.keys()) f[flags + flagIndex(addr)] |= 1;
    hs('active', breaks.size || actions.size || observers.size || hs('tracing') ? 1 : 0);
  }

  function postScreen(force) {
    const now = Date.now();
    if (!lastPixels || (!force && now - lastScreenPost < 100)) return;
    lastScreenPost = now;
    const rgba = rgba555(new Uint16Array(mem.buffer, lastPixels, 240 * 160));
    parentPort.postMessage({ type: 'screen', rgba: rgba.buffer }, [rgba.buffer]);
  }

  function status() {
    const d = dv();
    const gs = d.getUint32(0x02000710, true);
    const r = { frame: frameNo, stop, tracing: !!hs('tracing'), traceEvents: hs('count'),
      breaks: [...breaks].map(([a, b]) => ({ address: a, ...b })), queued: [...actions.keys()] };
    if (gs >= 0x02000000 && gs < 0x02040000) {
      r.scene = d.getUint32(gs + 0x5a4, true);
      r.player = { x: d.getInt16(gs + 0x30, true), y: d.getInt16(gs + 0x32, true), z: d.getInt16(gs + 0x34, true) };
      r.life = d.getUint16(gs + 0x28, true);
    }
    return r;
  }

  const imports = { env: {
    frame(ptr, n) {
      frameNo = n + 1;
      lastPixels = ptr;
      if (serving) return keys; // a function called from a command waited for VBlank
      if (Atomics.load(ctl, 1)) { Atomics.store(ctl, 1, 0); if (run) run.interrupted = true, run.left = 0, run.index = run.steps.length; }
      if (run) {
        while (run.left === 0 && run.index < run.steps.length) {
          [keys, run.left] = run.steps[run.index++];
        }
        if (run.left > 0) {
          run.left--;
          postScreen(false);
          return keys;
        }
        const r = run; run = null; keys = 0;
        stop = { reason: r.interrupted ? 'interrupted' : 'frame' };
        reply(r.id, status());
      }
      stop = stop || { reason: 'frame' };
      postScreen(true);
      serve();
      return keys;
    },
    hook(kind, address) {
      if (serving) return;
      const queued = actions.get(address);
      if (kind === 1 && queued) {
        actions.delete(address);
        refreshFlags();
        for (const cmd of queued) {
          const result = execute(cmd);
          if (cmd.id) reply(cmd.id, result);
        }
      }
      const peeks = kind === 1 && observers.get(address);
      if (peeks && observed.length < OBSERVE_LIMIT) {
        const d = dv();
        const args = [0, 1, 2, 3].map((i) => d.getUint32(hookState + 4 * (S.args + i), true));
        // the bytes some arguments point at (e.g. a script command's id)
        const bytes = peeks.map(([arg, off, len]) => {
          const at = args[arg] + off;
          return at >= 0x02000000 && at + len <= 0x0a000000 ? Buffer.from(u8().slice(at, at + len)).toString('hex') : null;
        });
        observed.push([frameNo, address, args, bytes]);
      }
      const b = breaks.get(address);
      if (!b || !(kind === 1 ? b.enter : b.exit)) return;
      if (run && !run.untilBreak) return;
      const d = dv();
      const args = [0, 1, 2, 3].map((i) => d.getUint32(hookState + 4 * (S.args + i), true));
      stop = { reason: 'break', kind: kind === 1 ? 'enter' : 'exit', address, args, depth: hs('depth') };
      if (run) { const r = run; run = null; keys = 0; reply(r.id, status()); }
      postScreen(true);
      serve();
    },
    audio() {},
    log(msg, value) { output.push(`[game] ${cstr(msg)} 0x${(value >>> 0).toString(16)}`); },
    fatal(msg, value) { output.push(`[fatal] ${cstr(msg)} 0x${(value >>> 0).toString(16)}`); },
  } };
  function cstr(p) { const b = u8(); let e = p; while (b[e]) e++; return Buffer.from(b.slice(p, e)).toString(); }

  function reply(id, result) {
    if (id && !replaying) parentPort.postMessage({ type: 'reply', id, result });
  }

  // Serve commands until one resumes the game (run / continue / restart).
  function serve() {
    for (;;) {
      if (replayQueue) {
        const cmd = replayQueue.shift();
        if (!replayQueue.length) replayQueue = null;
        if (cmd.op === 'run' || cmd.op === 'continue') { startRun({ ...cmd, id: null }); return; }
        execute(cmd);
        continue;
      }
      replaying = false;
      if (pendingReply) {
        const p = pendingReply;
        pendingReply = null;
        reply(p.id, p.error ? { error: p.error } : status());
      }
      const m = receiveMessageOnPort(commandPort);
      if (!m) {
        Atomics.wait(ctl, 0, 0);
        Atomics.store(ctl, 0, 0);
        continue;
      }
      {
        const cmd = m.message;
        if (cmd.op === 'run' || cmd.op === 'continue') {
          startRun(cmd); // commands sent after it wait for the next stop
          return;
        }
        if (cmd.op === 'restart' || cmd.op === 'restore') throw new Restart(JSON.stringify(cmd));
        reply(cmd.id, execute(cmd));
      }
    }
  }

  function startRun(cmd) {
    const steps = cmd.op === 'continue' ? [[0, cmd.frames ?? 600]] : (cmd.steps || [[cmd.keys || 0, cmd.frames ?? 1]]);
    if (!replaying) log.push(cmd);
    run = { id: cmd.id, steps: steps.map(([k, f]) => [typeof k === 'string' ? keyMask(k) : k, f]),
      index: 0, left: 0, untilBreak: cmd.op === 'continue' || !!cmd.untilBreak };
    stop = null;
    return true;
  }

  function keyMask(spec) {
    return spec.split(/[+&,]/).filter(Boolean).reduce((m, k) => {
      if (!(k.toLowerCase() in BUTTONS)) throw new Error('unknown key ' + k);
      return m | (1 << BUTTONS[k.toLowerCase()]);
    }, 0);
  }

  // Commands that run while the game is stopped. Mutations are logged for checkpoints.
  const MUTATING = new Set(['write', 'call', 'snippet', 'at', 'break', 'clear']);
  function execute(cmd) {
    const saved = { suspend: hs('suspend') };
    serving = true;
    output = [];
    try {
      if (MUTATING.has(cmd.op) && !replaying) log.push(cmd);
      const result = run1(cmd);
      if (output.length) result.log = output;
      return result;
    } catch (e) {
      return { error: String(e && e.message || e), log: output };
    } finally {
      serving = false;
      hs('suspend', saved.suspend);
    }
  }

  function run1(cmd) {
    switch (cmd.op) {
      case 'status': return status();
      case 'read': {
        const bytes = u8().slice(cmd.address, cmd.address + cmd.length);
        return { data: Buffer.from(bytes).toString('base64') };
      }
      case 'write': {
        u8().set(Buffer.from(cmd.data, 'base64'), cmd.address);
        return { ok: true };
      }
      case 'call': {
        hs('suspend', cmd.trace ? 0 : 1);
        const a = (cmd.args || []).map((x) => x >>> 0);
        while (a.length < 12) a.push(0);
        if (!ex.port_code_known(cmd.address)) throw new Error('no code at that address');
        const r = ex.port_icall(cmd.address, ...a);
        return { result: Number(BigInt.asUintN(32, r)), high: Number(BigInt.asUintN(32, r >> 32n)) };
      }
      case 'snippet': {
        hs('suspend', cmd.trace ? 0 : 1);
        const text = [];
        const mod = new WebAssembly.Module(Buffer.from(cmd.wasm, 'base64'));
        const env = { memory: mem, port_icall: ex.port_icall,
          agent_print: (p, n) => text.push(Buffer.from(u8().slice(p, p + n)).toString('latin1')) };
        for (const k of Object.keys(ex)) if (k.startsWith('port_bus_')) env[k] = ex[k];
        const inst = new WebAssembly.Instance(mod, { env });
        let result;
        try {
          result = inst.exports.snippet_main() >>> 0;
        } catch (e) {
          return { error: String(e && e.message || e), output: text.join('') };
        }
        return { output: text.join(''), result };
      }
      case 'break': {
        const b = breaks.get(cmd.address) || { enter: false, exit: false };
        b[cmd.kind || 'enter'] = cmd.on !== false;
        if (!b.enter && !b.exit) { breaks.delete(cmd.address); u8()[flags + flagIndex(cmd.address)] = 0; }
        else breaks.set(cmd.address, b);
        refreshFlags();
        return status();
      }
      case 'clear': {
        for (const a of breaks.keys()) u8()[flags + flagIndex(a)] = 0;
        breaks.clear();
        refreshFlags();
        return status();
      }
      case 'at': {
        // one-shot: run cmd.action at the next entry of cmd.address (the reply comes then)
        const list = actions.get(cmd.address) || [];
        list.push({ ...cmd.action, id: null });
        actions.set(cmd.address, list);
        refreshFlags();
        return { queued: true };
      }
      case 'trace': {
        if (cmd.on) {
          hs('count', 0); hs('overflow', 0); hs('inside', 0);
          hs('root', cmd.root || 0);
          hs('tracing', 1);
          refreshFlags();
          return { tracing: true };
        }
        hs('tracing', 0);
        refreshFlags();
        const count = hs('count');
        const bytes = new Uint8Array(mem.buffer, ex.port_hook_ring(), count * 24);
        if (cmd.path) fs.writeFileSync(cmd.path, bytes);
        return { events: count, overflow: hs('overflow') };
      }
      case 'shot': {
        if (!lastPixels) throw new Error('no frame yet');
        const data = png(rgba555(new Uint16Array(mem.buffer, lastPixels, 240 * 160)), 240, 160);
        fs.writeFileSync(cmd.path, data);
        return { path: cmd.path };
      }
      case 'dump': {
        fs.writeFileSync(cmd.path, u8().slice(cmd.address, cmd.address + cmd.length));
        return { path: cmd.path };
      }
      case 'checkpoint': {
        checkpoints.set(cmd.name, { log: log.slice(), save: currentSave, frame: frameNo });
        return { checkpoint: cmd.name, frame: frameNo, commands: log.length };
      }
      case 'observe': {
        if (cmd.on) {
          observers.clear();
          for (const p of cmd.points) observers.set(p.address, p.peek || []);
          observed = [];
          refreshFlags();
          return { observing: observers.size };
        }
        for (const a of observers.keys()) if (!breaks.has(a)) u8()[flags + flagIndex(a)] = 0;
        observers.clear();
        refreshFlags();
        const records = observed;
        observed = [];
        return { records, full: records.length >= OBSERVE_LIMIT };
      }
      case 'checkpoints':
        return { checkpoints: [...checkpoints].map(([name, c]) => ({ name, frame: c.frame, commands: c.log.length })) };
      default:
        throw new Error('unknown op ' + cmd.op);
    }
  }

  function boot() {
    const instance = new WebAssembly.Instance(new WebAssembly.Module(wasmBytes), imports);
    ex = instance.exports;
    mem = ex.memory;
    if (ex.port_memory_init() !== 0) throw new Error('memory init failed');
    u8().set(romBytes, 0x08000000);
    if (ex.port_load_rom(0x08000000, romBytes.length) !== 0) throw new Error('ROM load failed');
    ex.port_game_set_time(Math.floor(Date.parse(workerData.time) / 1000));
    ex.port_game_set_solar(solar);
    hookState = ex.port_hook_state();
    flags = ex.port_hook_flags();
    let saveSize = 0;
    if (currentSave) {
      u8().set(currentSave, ex.port_game_save_stage());
      saveSize = currentSave.length;
    }
    frameNo = 0; lastPixels = 0; keys = 0; run = null; stop = { reason: 'start' };
    breaks.clear(); actions.clear();
    return saveSize;
  }

  // Restart / restore: unwind the game (Restart), boot again, then at the
  // first frame stop replay the checkpoint's log (the game is deterministic)
  // and answer the request.
  for (;;) {
    try {
      const saveSize = boot();
      ex.port_game_run(saveSize);
      throw new Error('port_game_run returned');
    } catch (e) {
      if (!(e instanceof Restart)) {
        if (pendingReply) parentPort.postMessage({ type: 'reply', id: pendingReply.id, result: { error: 'game stopped: ' + String(e && e.stack || e) } });
        throw e;
      }
      const cmd = JSON.parse(e.message);
      log = [];
      replayQueue = null;
      pendingReply = { id: cmd.id };
      if (cmd.op === 'restart') {
        if (cmd.save !== undefined) currentSave = cmd.save ? fs.readFileSync(cmd.save) : null;
      } else {
        const c = checkpoints.get(cmd.name);
        if (!c) pendingReply.error = 'no checkpoint ' + cmd.name;
        else {
          currentSave = c.save;
          replayQueue = c.log.slice();
          log = c.log.slice();
          replaying = true;
        }
      }
    }
  }
}
