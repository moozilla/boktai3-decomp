// The live game as JavaScript objects: typed views of every global and struct
// (build/port/agent/types.json, from port/agent/gametypes.py), game functions
// callable by name, JS callbacks on any function's entry or exit, and scripts
// that step frames with `yield`. Shared by the agent daemon (Node) and the
// browser page's game worker; the host supplies memory, exports and frames.
//
//   const g = createGame({ memory, exports, types, print });
//   g.$.gPlayers[0].position.x            // typed read through pointers
//   g.$.gPlayers[0].position.x = 1700     // typed write
//   g.fn.Script_Call(945, 0)              // call a game function
//   g.on('Text_LookupString', (a) => print(a[0]))
//   g.spawn(function* () { yield g.press('down', 30); return g.$.gPlayers[0].position; })

const BUTTONS = { a: 0, b: 1, select: 2, start: 3, right: 4, left: 5, up: 6, down: 7, r: 8, l: 9 };
const VIEW = Symbol('view');
const INSPECT = Symbol.for('nodejs.util.inspect.custom');
const HS = { active: 0, tracing: 1, suspend: 2, depth: 14, args: 10 };

export function keyMask(keys) {
  if (typeof keys === 'number') return keys >>> 0;
  if (!keys) return 0;
  return String(keys).split(/[+&, ]/).filter(Boolean).reduce((m, k) => {
    const b = BUTTONS[k.toLowerCase()];
    if (b === undefined) throw new Error('unknown key ' + k + ' (a b select start right left up down r l)');
    return m | (1 << b);
  }, 0);
}

const hex = (n, w = 8) => '0x' + (n >>> 0).toString(16).padStart(w, '0');

export function createGame(env) {
  const { memory, exports: ex, types: T } = env;
  const out = env.print || ((text) => console.log(text));
  const print = (...a) => out(a.map((x) => (typeof x === 'string' ? x : format(x))).join(' '));
  const hookState = ex.port_hook_state();
  const flagBase = ex.port_hook_flags();
  let dv = new DataView(memory.buffer), bytes = new Uint8Array(memory.buffer);
  const mem = () => {
    if (dv.buffer !== memory.buffer) { dv = new DataView(memory.buffer); bytes = new Uint8Array(memory.buffer); }
    return dv;
  };

  // ---- symbols ------------------------------------------------------------
  const functions = T.functions || {};
  const globals = T.globals || {};
  const data = T.data || {};
  let byAddress = null;
  function addressTable() {
    if (byAddress) return byAddress;
    const rows = [];
    for (const [n, a] of Object.entries(functions)) rows.push([a & ~1, n, 'fn']);
    for (const [n, gl] of Object.entries(globals)) rows.push([gl.addr, n, 'global']);
    for (const [n, a] of Object.entries(data)) rows.push([a, n, 'data']);
    rows.sort((x, y) => x[0] - y[0]);
    byAddress = rows;
    return rows;
  }
  function addr(where) {
    if (typeof where === 'number') return where >>> 0;
    if (where && where[VIEW]) return where[VIEW].addr;
    if (typeof where === 'string') {
      if (/^(0x)?[0-9a-f]+$/i.test(where) && /^0x|^[0-9]+$/i.test(where)) return Number(where) >>> 0;
      if (where in functions) return functions[where] >>> 0;
      if (where in globals) return globals[where].addr;
      if (where in data) return data[where];
      throw new Error('unknown symbol ' + where);
    }
    throw new Error('not an address: ' + where);
  }
  function name(a) {
    a >>>= 0;
    const rows = addressTable();
    let lo = 0, hi = rows.length - 1, best = -1;
    while (lo <= hi) { const m = (lo + hi) >> 1; if (rows[m][0] <= (a & ~1)) { best = m; lo = m + 1; } else hi = m - 1; }
    if (best < 0) return hex(a);
    const [base, n] = rows[best];
    const off = (a & ~1) - base;
    return off ? `${n}+0x${off.toString(16)}` : n;
  }
  function find(pattern) {
    const re = pattern instanceof RegExp ? pattern : new RegExp(pattern, 'i');
    return addressTable().filter(([, n]) => re.test(n)).slice(0, 200).map(([a, n, k]) => ({ name: n, addr: hex(a), kind: k }));
  }

  // ---- types --------------------------------------------------------------
  function resolve(t) {
    for (let i = 0; i < 32 && typeof t === 'string'; i++) {
      const d = T.types[t];
      if (!d) return { k: 'void', name: t };
      if (d.k !== 'typedef') return Object.assign(d, { name: d.name || t });
      t = d.to;
    }
    return typeof t === 'string' ? { k: 'void', name: t } : t;
  }
  function typeName(t) {
    if (typeof t === 'string') return t;
    if (!t) return 'void';
    if (t.k === 'ptr') return typeName(t.to) + ' *';
    if (t.k === 'array') return typeName(t.of) + `[${t.n}]`;
    if (t.k === 'fn') return 'fn';
    return t.name || `${t.k} {…}`;
  }
  function sizeof(t) {
    const r = resolve(t);
    switch (r.k) {
      case 'int': case 'float': case 'enum': return r.size;
      case 'ptr': return 4;
      case 'array': return r.n * sizeof(r.of);
      case 'struct': case 'union': return r.size;
      default: return 1;
    }
  }
  const fieldMaps = new WeakMap();
  function fieldsOf(r) {
    let m = fieldMaps.get(r);
    if (m) return m;
    m = new Map();
    const add = (fields, base) => {
      for (const f of fields) {
        if (!f.name) { const s = resolve(f.type); if (s.fields) add(s.fields, base + f.off); continue; }
        m.set(f.name, base ? { ...f, off: f.off + base } : f);
      }
    };
    add(r.fields || [], 0);
    fieldMaps.set(r, m);
    return m;
  }
  function parseType(text) {
    // 'struct Player', 'u16', 'struct Task *', 'u8[16]'
    let s = String(text).trim();
    const arr = s.match(/^(.*)\[(\d+)\]$/);
    if (arr) return { k: 'array', of: parseType(arr[1]), n: Number(arr[2]) };
    if (s.endsWith('*')) return { k: 'ptr', to: parseType(s.slice(0, -1)) };
    if (!(s in T.types) && ('struct ' + s) in T.types) s = 'struct ' + s;
    if (!(s in T.types)) throw new Error('unknown type ' + text);
    return s;
  }

  // ---- memory -------------------------------------------------------------
  const valid = (a) => a >= 0x02000000 && a < 0x0a000000;
  function readInt(a, size, signed) {
    const d = mem();
    if (size === 1) return signed ? d.getInt8(a) : d.getUint8(a);
    if (size === 2) return signed ? d.getInt16(a, true) : d.getUint16(a, true);
    return signed ? d.getInt32(a, true) : d.getUint32(a, true);
  }
  function writeInt(a, size, v) {
    const d = mem();
    if (size === 1) d.setUint8(a, v & 0xff);
    else if (size === 2) d.setUint16(a, v & 0xffff, true);
    else d.setUint32(a, v >>> 0, true);
  }
  function read(a, t) {
    const r = resolve(t);
    switch (r.k) {
      case 'int': return readInt(a, r.size, r.signed);
      case 'enum': return readInt(a, r.size, r.signed !== false);
      case 'float': return r.size === 4 ? mem().getFloat32(a, true) : mem().getFloat64(a, true);
      case 'ptr': {
        const p = readInt(a, 4, false);
        if (!p) return null;
        const to = resolve(r.to);
        if (valid(p) && (to.k === 'struct' || to.k === 'union' || to.k === 'array')) return view(p, r.to);
        return p;
      }
      case 'struct': case 'union': case 'array': return view(a, t);
      default: return readInt(a, 4, false);
    }
  }
  function write(a, t, v) {
    const r = resolve(t);
    if (v && v[VIEW]) {
      if (r.k === 'ptr') return writeInt(a, 4, v[VIEW].addr);
      bytes.copyWithin(a, v[VIEW].addr, v[VIEW].addr + sizeof(t));
      return;
    }
    if (r.k === 'struct' || r.k === 'union' || r.k === 'array') {
      if (typeof v !== 'object') throw new Error(`cannot assign ${v} to ${typeName(t)}`);
      const target = view(a, t);
      for (const [k, x] of Object.entries(v)) target[k] = x;
      return;
    }
    if (r.k === 'enum' && typeof v === 'string') v = r.values[v];
    if (r.k === 'float') return r.size === 4 ? mem().setFloat32(a, v, true) : mem().setFloat64(a, v, true);
    writeInt(a, r.k === 'ptr' ? 4 : r.size || 4, v == null ? 0 : v);
  }
  function readField(a, f) {
    if (f.bits) {
      const word = readInt(a + f.off, 4, false);
      const r = resolve(f.type);
      let v = (word >>> f.bit) & (f.bits === 32 ? 0xffffffff : (1 << f.bits) - 1);
      if (r.signed && v & (1 << (f.bits - 1))) v -= 1 << f.bits;
      return v;
    }
    return read(a + f.off, f.type);
  }
  function writeField(a, f, v) {
    if (f.bits) {
      const mask = (f.bits === 32 ? 0xffffffff : (1 << f.bits) - 1) << f.bit;
      const word = readInt(a + f.off, 4, false);
      return writeInt(a + f.off, 4, (word & ~mask) | ((v << f.bit) & mask));
    }
    write(a + f.off, f.type, v);
  }

  // ---- views --------------------------------------------------------------
  function view(a, t) {
    const r = resolve(t);
    const info = { addr: a >>> 0, type: t, r };
    const helpers = {
      $addr: info.addr, $type: typeName(t), $size: sizeof(t),
      $get: (depth = 1) => snapshot(info.addr, t, depth),
      $show: (depth = 1) => show(info.addr, t, depth),
      $at: (i) => view(info.addr + i * sizeof(t), t),
      $cast: (to) => view(info.addr, parseType(to)),
      toJSON: () => snapshot(info.addr, t, 1),
      toString: () => `${typeName(t)} @ ${hex(info.addr)}`,
    };
    const target = r.k === 'array' ? [] : {};
    return new Proxy(target, {
      get(_, key) {
        if (key === VIEW) return info;
        if (key === INSPECT) return () => show(info.addr, t, 1);
        if (key in helpers) return helpers[key];
        if (r.k === 'array') {
          if (key === 'length') return r.n;
          if (key === Symbol.iterator) return function* () { for (let i = 0; i < r.n; i++) yield read(info.addr + i * sizeof(r.of), r.of); };
          if (typeof key === 'string' && /^\d+$/.test(key)) return read(info.addr + Number(key) * sizeof(r.of), r.of);
          if (typeof Array.prototype[key] === 'function') return (...args) => Array.prototype[key].apply([...this.get(_, Symbol.iterator)()], args);
          return undefined;
        }
        if (typeof key !== 'string') return undefined;
        const f = fieldsOf(r).get(key);
        return f ? readField(info.addr, f) : undefined;
      },
      set(_, key, v) {
        if (r.k === 'array' && /^\d+$/.test(key)) { write(info.addr + Number(key) * sizeof(r.of), r.of, v); return true; }
        const f = fieldsOf(r).get(key);
        if (!f) throw new Error(`${typeName(t)} has no field ${String(key)}`);
        writeField(info.addr, f, v);
        return true;
      },
      has(_, key) { return r.k === 'array' ? /^\d+$/.test(key) && Number(key) < r.n : fieldsOf(r).has(key); },
      ownKeys() { return r.k === 'array' ? [...Array(r.n).keys()].map(String).concat('length') : [...fieldsOf(r).keys()]; },
      getOwnPropertyDescriptor(_, key) {
        if (r.k === 'array' && key === 'length') return { value: r.n, writable: true, enumerable: false, configurable: false };
        return { enumerable: true, configurable: true, writable: true, value: this.get(_, key) };
      },
    });
  }
  function at(where, type) { return view(addr(where), parseType(type)); }

  function scalarText(v, t) {
    const r = resolve(t);
    if (v === null) return 'NULL';
    if (v && v[VIEW]) return `${hex(v[VIEW].addr)} → ${typeName(v[VIEW].type)}`;
    if (r.k === 'ptr') return hex(v) + (resolve(r.to).k === 'fn' || v >= 0x08000000 ? ' ' + name(v) : '');
    if (r.k === 'enum') { const e = Object.entries(r.values).find(([, x]) => x === v); return e ? `${e[0]} (${v})` : String(v); }
    if (r.k === 'int' && r.size === 4 && !r.signed && v > 0xffff) return hex(v);
    return String(v);
  }
  function snapshot(a, t, depth) {
    const r = resolve(t);
    if (r.k === 'array') {
      const er = resolve(r.of);
      if (er.k === 'int' && er.size === 1 && r.n > 16) return Array.from(bytes.slice(a, a + r.n), (b) => b.toString(16).padStart(2, '0')).join(' ');
      const out = [];
      for (let i = 0; i < Math.min(r.n, 256); i++) out.push(value(a + i * sizeof(r.of), r.of, depth));
      return out;
    }
    const out = {};
    for (const [k, f] of fieldsOf(r)) out[k] = f.bits ? readField(a, f) : value(a + f.off, f.type, depth);
    return out;
  }
  function value(a, t, depth) {
    const r = resolve(t);
    if (r.k === 'struct' || r.k === 'union' || r.k === 'array') return depth > 0 ? snapshot(a, t, depth - 1) : `${typeName(t)} @ ${hex(a)}`;
    const v = read(a, t);
    if (r.k === 'ptr' || r.k === 'enum') return scalarText(v, t);
    return v;
  }
  function show(a, t, depth = 1, indent = '') {
    const r = resolve(t);
    const lines = indent ? [] : [`${typeName(t)} @ ${hex(a)} (${sizeof(t)} bytes)`];
    const pad = indent + '  ';
    if (r.k === 'array') {
      const er = resolve(r.of);
      if (er.k === 'int' || er.k === 'enum' || er.k === 'ptr') {
        const vals = [];
        for (let i = 0; i < Math.min(r.n, 64); i++) vals.push(er.k === 'int' && er.size === 1 ? (bytes[a + i]).toString(16).padStart(2, '0') : scalarText(read(a + i * sizeof(r.of), r.of), r.of));
        lines.push(pad + vals.join(er.size === 1 ? ' ' : ', ') + (r.n > 64 ? ' …' : ''));
      } else {
        for (let i = 0; i < Math.min(r.n, 32); i++) {
          lines.push(`${pad}[${i}] ${typeName(r.of)}`);
          if (depth > 0) lines.push(...show(a + i * sizeof(r.of), r.of, depth - 1, pad + '  ').split('\n'));
        }
      }
      return lines.join('\n');
    }
    const width = Math.max(4, ...[...fieldsOf(r).keys()].map((k) => k.length));
    for (const [k, f] of fieldsOf(r)) {
      const fr = resolve(f.type);
      const head = `${pad}+${f.off.toString(16).padStart(3, '0')} ${k.padEnd(width)} `;
      if (f.bits) { lines.push(head + `${typeName(f.type)}:${f.bits} = ${readField(a, f)}`); continue; }
      if (fr.k === 'struct' || fr.k === 'union') {
        if (depth > 0) {
          const inner = [...fieldsOf(fr)].filter(([, g]) => { const gr = resolve(g.type); return gr.k !== 'struct' && gr.k !== 'union' && gr.k !== 'array'; });
          if (inner.length === fieldsOf(fr).size && inner.length <= 6) {
            lines.push(head + `${typeName(f.type)} { ` + inner.map(([n, g]) => `${n}: ${scalarText(g.bits ? readField(a + f.off, g) : read(a + f.off + g.off, g.type), g.type)}`).join(', ') + ' }');
          } else {
            lines.push(head + typeName(f.type));
            lines.push(show(a + f.off, f.type, depth - 1, pad + '  '));
          }
        } else lines.push(head + typeName(f.type) + ' …');
        continue;
      }
      if (fr.k === 'array') {
        const er = resolve(fr.of);
        if ((er.k === 'int' || er.k === 'enum' || er.k === 'ptr') && depth >= 0) {
          lines.push(head + typeName(f.type) + ' ' + show(a + f.off, f.type, depth, '').split('\n').slice(1).join(' ').trim());
        } else {
          lines.push(head + typeName(f.type) + (depth > 0 ? '' : ' …'));
          if (depth > 0) lines.push(show(a + f.off, f.type, depth - 1, pad + '  '));
        }
        continue;
      }
      lines.push(head + `${typeName(f.type)} = ${scalarText(read(a + f.off, f.type), f.type)}`);
    }
    return lines.filter((l) => l !== '').join('\n');
  }

  // Globals as live values: g.$.gPlayers[0].position.x
  const $ = new Proxy({}, {
    get(_, key) {
      if (typeof key !== 'string') return undefined;
      const gl = globals[key];
      if (gl) return read(gl.addr, gl.type);
      if (key in data) return data[key];
      return undefined;
    },
    set(_, key, v) {
      const gl = globals[key];
      if (!gl) throw new Error('no typed global ' + key);
      write(gl.addr, gl.type, v);
      return true;
    },
    has(_, key) { return key in globals; },
    ownKeys() { return Object.keys(globals); },
    getOwnPropertyDescriptor() { return { enumerable: true, configurable: true }; },
  });

  // ---- calls --------------------------------------------------------------
  function call(fn, ...args) {
    const a = addr(fn);
    if (!ex.port_code_known(a)) throw new Error('no code at ' + hex(a));
    const words = args.map((x) => (x && x[VIEW] ? x[VIEW].addr : x === true ? 1 : x || 0) >>> 0);
    while (words.length < 12) words.push(0);
    const r = ex.port_icall(a, ...words);
    return Number(BigInt.asIntN(32, r));
  }
  const fn = new Proxy({}, {
    get(_, key) { if (typeof key === 'string' && key in functions) return (...args) => call(key, ...args); return undefined; },
    has(_, key) { return key in functions; },
    ownKeys() { return Object.keys(functions); },
    getOwnPropertyDescriptor() { return { enumerable: true, configurable: true }; },
  });

  // ---- hooks --------------------------------------------------------------
  // g.on(fn, cb, {exit}) runs cb(args, info) synchronously inside the game at
  // every entry (args = r0..r3) or exit (args = [result]). Return 'stop' to
  // stop the game there (the host decides what stopping means).
  const hooks = new Map(); // addr -> {enter: [h], exit: [h]}
  let hookId = 1;
  const hs = (k, v) => {
    const a = hookState + 4 * HS[k];
    if (v === undefined) return mem().getUint32(a, true);
    mem().setUint32(a, v >>> 0, true);
  };
  const flagIndex = (a) => {
    if (a >= 0x03000000 && a < 0x0300110c) a = a - 0x03000000 + 0x08000350;
    return (a - 0x08000000) >>> 1;
  };
  function armFlags() {
    mem();
    for (const [a, h] of hooks) bytes[flagBase + flagIndex(a)] |= (h.enter.length ? 1 : 0) | (h.exit.length ? 2 : 0);
    return hooks.size > 0;
  }
  function refresh() {
    if (env.refresh) env.refresh();
    else { armFlags(); hs('active', hooks.size || hs('tracing') ? 1 : 0); }
  }
  function on(where, cb, opts = {}) {
    const a = addr(where) & ~1;
    const h = hooks.get(a) || { enter: [], exit: [] };
    const entry = { id: hookId++, cb, once: !!opts.once, where: typeof where === 'string' ? where : name(a) };
    h[opts.exit ? 'exit' : 'enter'].push(entry);
    hooks.set(a, h);
    refresh();
    return entry.id;
  }
  function off(id) {
    for (const [a, h] of hooks) {
      for (const k of ['enter', 'exit']) h[k] = id === undefined ? [] : h[k].filter((e) => e.id !== id);
      if (!h.enter.length && !h.exit.length) { hooks.delete(a); mem(); bytes[flagBase + flagIndex(a)] = 0; }
    }
    refresh();
  }
  function hookList() {
    const out = [];
    for (const [a, h] of hooks) for (const k of ['enter', 'exit']) for (const e of h[k]) out.push({ id: e.id, fn: e.where, addr: hex(a), kind: k });
    return out;
  }
  // Called by the host's hook import. Returns true if a callback asked to stop.
  function dispatch(kind, a) {
    const h = hooks.get(a);
    const waiting = tasks.length && tasks.some((t) => t.wait && t.wait.call === a && t.wait.kind === kind);
    if (!h && !waiting) return false;
    const d = mem();
    const args = kind === 1 ? [0, 1, 2, 3].map((i) => d.getUint32(hookState + 4 * (HS.args + i), true))
      : [d.getUint32(hookState + 4 * HS.args, true)];
    let stop = false;
    const list = h && (kind === 1 ? h.enter : h.exit);
    if (list && list.length) {
      const info = { kind: kind === 1 ? 'enter' : 'exit', fn: name(a), addr: a, depth: hs('depth'), frame: frameNo };
      for (const e of list.slice()) {
        let r;
        try { r = e.cb(args, info); } catch (err) { print(`[hook ${e.where}] ${err && err.stack || err}`); r = undefined; }
        if (e.once) off(e.id);
        if (r === 'stop') stop = true;
      }
    }
    if (waiting) {
      for (const t of tasks.slice()) {
        if (t.wait && t.wait.call === a && t.wait.kind === kind) step(t, { args, frame: frameNo });
      }
    }
    return stop;
  }

  // ---- scripts ------------------------------------------------------------
  // A script is a generator: `yield N` runs N frames, `yield g.press('down', 30)`
  // holds keys, `yield g.until(() => cond, max)` runs until cond (checked every
  // frame), `yield g.untilCall('Fn')` runs until Fn is entered and returns its
  // arguments; yield another generator (or yield*) to run it inline.
  let frameNo = 0;
  let heldKeys = 0;
  const tasks = [];
  function press(keys, frames = 1) { return { frames, keys: keyMask(keys) }; }
  function tap(keys) {
    return (function* () { yield press(keys, 2); yield press(0, 6); })();
  }
  function until(cond, max = 3600) { return { until: cond, max }; }
  function untilCall(where, opts = {}) { return { call: addr(where) & ~1, kind: opts.exit ? 2 : 1, max: opts.max ?? 36000 }; }
  function spawn(gen, opts = {}) {
    const it = typeof gen === 'function' ? gen() : gen;
    const t = { stack: [it], wait: null, done: false, result: undefined, error: null, name: opts.name || 'script', keys: 0, onDone: opts.onDone };
    tasks.push(t);
    step(t, undefined);
    return t;
  }
  function finish(t, result, error) {
    t.done = true; t.result = result; t.error = error; t.wait = null;
    const i = tasks.indexOf(t);
    if (i >= 0) tasks.splice(i, 1);
    refresh();
    if (t.onDone) t.onDone(t);
  }
  function step(t, input) {
    t.wait = null;
    for (;;) {
      const it = t.stack[t.stack.length - 1];
      let r;
      try { r = it.next(input); } catch (e) { finish(t, undefined, e); return; }
      if (r.done) {
        t.stack.pop();
        if (!t.stack.length) { finish(t, r.value, null); return; }
        input = r.value;
        continue;
      }
      let w = r.value;
      if (w && typeof w.next === 'function') { t.stack.push(w); input = undefined; continue; }
      if (w === undefined || typeof w === 'number') w = { frames: w ?? 1 };
      if (w.frames !== undefined) {
        if (w.frames <= 0) { input = undefined; continue; }
        t.wait = { frames: w.frames, keys: w.keys };
      } else if (w.until) {
        let ok;
        try { ok = w.until(); } catch (e) { finish(t, undefined, e); return; }
        if (ok) { input = true; continue; }
        t.wait = { until: w.until, left: w.max };
      } else if (w.call !== undefined) {
        t.wait = { call: w.call, kind: w.kind, left: w.max };
        mem();
        bytes[flagBase + flagIndex(w.call)] |= w.kind;
        hs('active', 1);
      } else { finish(t, undefined, new Error('a script yielded ' + JSON.stringify(w))); return; }
      return;
    }
  }
  // Called by the host once per frame (VBlank). Returns the keys scripts hold.
  function frame(n) {
    frameNo = n;
    let keys = heldKeys;
    for (const t of tasks.slice()) {
      const w = t.wait;
      if (!w) continue;
      if (w.frames !== undefined) {
        if (--w.frames <= 0) step(t, undefined);
      } else if (w.until) {
        let ok;
        try { ok = w.until(); } catch (e) { finish(t, undefined, e); continue; }
        if (ok) step(t, true);
        else if (--w.left <= 0) step(t, false);
      } else if (w.call !== undefined && --w.left <= 0) step(t, null);
    }
    return keys | currentKeys();
  }
  function currentKeys() {
    let keys = heldKeys;
    for (const t of tasks) if (t.wait && t.wait.keys !== undefined) keys |= t.wait.keys;
    return keys;
  }

  // ---- library ------------------------------------------------------------
  // port/agent/lib/*.js: plain scripts that add functions to `exports`; every
  // script sees them by name (skills an agent writes once and reuses).
  const lib = {};
  function loadLib(sources) {
    for (const [file, text] of Object.entries(sources)) {
      try {
        new Function('g', '$', 'print', 'exports', text)(g, $, print, lib);
      } catch (e) {
        print(`[lib ${file}] ${e && e.stack || e}`);
      }
    }
  }
  function compile(code) {
    const names = Object.keys(lib).filter((k) => /^[A-Za-z_$][\w$]*$/.test(k));
    const make = (body) => new Function('g', '$', 'print', ...names, `"use strict";\nreturn function* script() {\n${body}\n};`);
    const args = [g, $, print, ...names.map((k) => lib[k])];
    // Without yield, run it as eval so the last expression is the result (as in a console).
    if (!/\byield\b/.test(code)) {
      const evaluator = new Function('g', '$', 'print', ...names, '"use strict"; return (code) => eval(code);')(...args);
      try {
        new Function(`return class { static {\n${code}\n} }`); // parse only: a top-level return needs the function form
        return function* script() { return evaluator(code); };
      } catch (e) { /* fall through */ }
    }
    let f;
    try { f = make(`return (${code}\n);`); } catch (e) { f = make(code); }
    return f(...args);
  }

  function format(v, depth = 1) {
    if (v && v[VIEW]) return show(v[VIEW].addr, v[VIEW].type, depth);
    if (v === undefined) return '';
    if (typeof v === 'string') return v;
    if (Array.isArray(v) && v.length && v.every((x) => x && typeof x === 'object' && !x[VIEW] && !Array.isArray(x)))
      return v.map((x) => Object.entries(x).map(([k, y]) => `${k}=${typeof y === 'object' ? JSON.stringify(y) : y}`).join('  ')).join('\n');
    if (typeof v === 'number') return Number.isInteger(v) && Math.abs(v) > 0xffff ? `${v} (${hex(v)})` : String(v);
    try {
      return JSON.stringify(v, (k, x) => (x && x[VIEW] ? x.toJSON() : typeof x === 'bigint' ? String(x) : x), 1)
        .replace(/\n\s*/g, ' ');
    } catch (e) { return String(v); }
  }

  const g = {
    $, fn, call, addr, name, find, at, view, read, write, sizeof, types: T, typeName, resolve,
    u8: (a, v) => (v === undefined ? readInt(addr(a), 1, false) : writeInt(addr(a), 1, v)),
    s8: (a) => readInt(addr(a), 1, true),
    u16: (a, v) => (v === undefined ? readInt(addr(a), 2, false) : writeInt(addr(a), 2, v)),
    s16: (a) => readInt(addr(a), 2, true),
    u32: (a, v) => (v === undefined ? readInt(addr(a), 4, false) : writeInt(addr(a), 4, v)),
    s32: (a) => readInt(addr(a), 4, true),
    bytes: (a, n) => { mem(); return bytes.slice(addr(a), addr(a) + n); },
    hex: (a, n = 64) => { mem(); const s = addr(a); return Array.from(bytes.slice(s, s + n), (b) => b.toString(16).padStart(2, '0')).join(' '); },
    cstr: (a, max = 256) => { mem(); const s = addr(a); let e = s; while (bytes[e] && e - s < max) e++; return String.fromCharCode(...bytes.slice(s, e)); },
    show: (v, depth = 1) => format(v, depth),
    on, off, hooks: hookList, dispatch, armFlags, hasHooks: () => hooks.size > 0 || tasks.some((t) => t.wait && t.wait.call !== undefined),
    press, tap, until, untilCall, spawn, frame, currentKeys, tasks,
    get keys() { return heldKeys; }, set keys(k) { heldKeys = keyMask(k); },
    get frameNo() { return frameNo; },
    lib, loadLib, compile, format, print, keyMask, hex,
    env,
  };
  return g;
}
