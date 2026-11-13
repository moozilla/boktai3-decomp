// Worker side of the actor panel (port/web/actor-panel.js): which task drew
// each sprite, the typed struct behind the one the player clicked, the sprite
// layer decoded from OAM and VRAM (with transparency), and the actors in world
// space for the 3D view. `g` is the gameapi object (port/agent/gameapi.mjs); memory
// addresses are GBA addresses.

const OBJ_SIZE = [[[8, 8], [16, 16], [32, 32], [64, 64]], [[16, 8], [32, 8], [32, 16], [64, 32]], [[8, 16], [8, 32], [16, 32], [32, 64]]];
const PADDING = /^(pad|unk|reserved|unused|_)/i;

export function createActorTools(g, memory) {
  let bytes = new Uint8Array(memory.buffer);
  const mem = () => { if (bytes.buffer !== memory.buffer) bytes = new Uint8Array(memory.buffer); return bytes; };
  const u16 = (a) => bytes[a] | (bytes[a + 1] << 8);
  const u32 = (a) => (bytes[a] | (bytes[a + 1] << 8) | (bytes[a + 2] << 16) | (bytes[a + 3] << 24)) >>> 0;

  // ---- the struct behind a task -------------------------------------------
  // A task's update function takes its own object, so its first parameter's
  // type (types.json "params") types the memory from the task header on.
  function ownerOf(addr) {
    const tasks = g.lib.tasks ? g.lib.tasks() : [];
    let best = null;
    for (const t of tasks) {
      const a = Number(t.task);
      if (a <= addr && addr - a < 0x2000 && (!best || a > Number(best.task))) best = t;
    }
    if (!best) return null;
    const type = (g.types.params || {})[best.update] || null;
    return { task: Number(best.task), update: best.update, type, id: best.id, list: best.list, dead: best.dead };
  }

  function flatFields(r, base = 0, out = []) {
    for (const f of r.fields || []) {
      if (!f.name) { const s = g.resolve(f.type); if (s.fields) flatFields(s, base + f.off, out); continue; }
      out.push(base ? { ...f, off: f.off + base } : f);
    }
    return out;
  }
  function scalar(a, t, f) {
    const r = g.resolve(t);
    if (f && f.bits) {
      const word = u32(a);
      let v = (word >>> f.bit) & (f.bits === 32 ? 0xffffffff : (1 << f.bits) - 1);
      if (r.signed && v & (1 << (f.bits - 1))) v -= 1 << f.bits;
      return { v, bits: f.bits };
    }
    if (r.k === 'int' || r.k === 'enum') {
      const size = r.size || 4, signed = r.k === 'enum' ? r.signed !== false : r.signed;
      let v = size === 1 ? bytes[a] : size === 2 ? u16(a) : u32(a);
      if (signed) v = size === 1 ? (v << 24) >> 24 : size === 2 ? (v << 16) >> 16 : v | 0;
      if (r.k === 'enum' && r.values) {
        const name = Object.keys(r.values).find((k) => r.values[k] === v);
        return { v, enumName: name || null };
      }
      return { v };
    }
    if (r.k === 'float') return { v: new DataView(memory.buffer).getFloat32(a, true) };
    if (r.k === 'ptr') {
      const p = u32(a);
      const to = g.resolve(r.to);
      const out = { v: p, ptr: true };
      if (p && (to.k === 'fn' || (p >= 0x08000000 && p < 0x0a000000))) { const n = g.name(p); if (n && !/^0x/.test(n)) out.sym = n; }
      return out;
    }
    return { v: u32(a) };
  }
  // A tree of {n, t, o, v?, c?, ...} the panel renders; `budget` caps the size.
  function tree(a, t, depth, budget, offset = 0) {
    const r = g.resolve(t);
    if (r.k === 'struct' || r.k === 'union') {
      const kids = [];
      for (const f of flatFields(r)) {
        if (budget.n-- <= 0) break;
        const node = { n: f.name, t: g.typeName(f.type), o: offset + f.off };
        if (PADDING.test(f.name)) node.pad = 1;
        const fr = g.resolve(f.type);
        if (fr.k === 'enum' && fr.values && budget.enums) budget.enums[node.t] = fr.values;
        if (f.bits || fr.k === 'int' || fr.k === 'enum' || fr.k === 'float' || fr.k === 'ptr') Object.assign(node, scalar(a + f.off, f.type, f));
        else if (depth > 0) node.c = tree(a + f.off, f.type, depth - 1, budget, offset + f.off);
        else node.more = 1;
        kids.push(node);
      }
      return kids;
    }
    if (r.k === 'array') {
      const er = g.resolve(r.of), size = g.sizeof(r.of), kids = [];
      const scalarEl = er.k === 'int' || er.k === 'enum' || er.k === 'ptr' || er.k === 'float';
      const n = Math.min(r.n, scalarEl ? 64 : 8);
      for (let i = 0; i < n; i++) {
        if (budget.n-- <= 0) break;
        const node = { n: i, t: g.typeName(r.of), o: offset + i * size };
        if (scalarEl) Object.assign(node, scalar(a + i * size, r.of));
        else if (depth > 0) node.c = tree(a + i * size, r.of, depth - 1, budget, offset + i * size);
        else node.more = 1;
        kids.push(node);
      }
      if (r.n > n) kids.push({ n: `… ${r.n - n} more`, t: '', o: offset + n * size, gap: 1 });
      return kids;
    }
    return [];
  }

  // ---- sprite layer: OAM + OBJ VRAM + OBJ palettes -> RGBA 240x160 ----------
  const layer = new Uint8ClampedArray(240 * 160 * 4);
  function spriteLayer() {
    mem();
    layer.fill(0);
    const dispcnt = u16(0x04000000);
    if (!(dispcnt & 0x1000)) return layer;
    const oneD = dispcnt & 0x40;
    const pal = new Uint32Array(256);
    for (let i = 0; i < 256; i++) {
      const c = u16(0x05000200 + 2 * i);
      const r = c & 31, gg = (c >> 5) & 31, b = (c >> 10) & 31;
      pal[i] = ((r << 3) | (r >> 2)) | (((gg << 3) | (gg >> 2)) << 8) | (((b << 3) | (b >> 2)) << 16) | 0xff000000;
    }
    const out = new Uint32Array(layer.buffer);
    // lower OAM index wins: draw from the back
    for (let i = 127; i >= 0; i--) {
      const o = 0x07000000 + 8 * i;
      const a0 = u16(o), a1 = u16(o + 2), a2 = u16(o + 4);
      const affine = a0 & 0x100;
      if (!affine && (a0 & 0x200)) continue;
      if (((a0 >> 10) & 3) === 2) continue; // object window
      const [w, h] = (OBJ_SIZE[a0 >> 14] || OBJ_SIZE[0])[a1 >> 14];
      let x = a1 & 0x1ff, y = a0 & 0xff;
      if (x >= 240) x -= 512;
      if (y >= 160) y -= 256;
      if (affine && (a0 & 0x200)) { x += w / 2; y += h / 2; } // double-size box: draw unrotated in its middle
      const bpp8 = a0 & 0x2000, tile = a2 & 0x3ff, bank = (a2 >> 12) * 16;
      const hflip = !affine && (a1 & 0x1000), vflip = !affine && (a1 & 0x2000);
      const tilesW = w >> 3;
      for (let py = 0; py < h; py++) {
        const sy = y + py;
        if (sy < 0 || sy >= 160) continue;
        const ty = vflip ? h - 1 - py : py, trow = ty >> 3;
        for (let px = 0; px < w; px++) {
          const sx = x + px;
          if (sx < 0 || sx >= 240) continue;
          const tx = hflip ? w - 1 - px : px, tcol = tx >> 3;
          let ci;
          if (bpp8) {
            const tn = (oneD ? tile + (trow * tilesW + tcol) * 2 : tile + trow * 32 + tcol * 2) & 0x3ff;
            ci = bytes[0x06010000 + tn * 32 + (ty & 7) * 8 + (tx & 7)];
          } else {
            const tn = (oneD ? tile + trow * tilesW + tcol : tile + trow * 32 + tcol) & 0x3ff;
            const b = bytes[0x06010000 + tn * 32 + (ty & 7) * 4 + ((tx & 7) >> 1)];
            ci = tx & 1 ? b >> 4 : b & 15;
            if (ci) ci += bank;
          }
          if (ci) out[sy * 240 + sx] = pal[ci];
        }
      }
    }
    return layer;
  }

  // ---- who drew what: OAM entries per task ----------------------------------
  // Tasks only queue their sprites; a render pass after them turns each sprite
  // object into OAM entries (sub_08000430 for IWRAM objects,
  // Sprite_QueueFramePieces for animated ones). Hooking those two and
  // comparing gOamCount / gPendingOamCount before and after gives every object
  // its entries; the task whose memory holds the object owns them.
  // (Projected SpriteDrawables are drawn in bulk; lib drawables() covers them.)
  const EMITTERS = ['sub_08000430', 'Sprite_QueueFramePieces'];
  const att = { ids: [], stack: [], frame: new Map(), last: new Map() };
  const G = (n) => g.types.globals[n] && g.types.globals[n].addr;
  const oam = { count: G('gOamCount'), buf: G('gOamBuffer'), pcount: G('gPendingOamCount'), pbuf: G('gPendingOamBuffer') };
  function boxOf(attr01) {
    const a0 = attr01 & 0xffff, a1 = attr01 >>> 16;
    if ((a0 & 0x300) === 0x200) return null;
    let [w, h] = (OBJ_SIZE[a0 >> 14] || OBJ_SIZE[0])[a1 >> 14];
    if ((a0 & 0x300) === 0x300) { w *= 2; h *= 2; }
    let y = a0 & 0xff, x = a1 & 0x1ff;
    if (y >= 160) y -= 256;
    if (x >= 240) x -= 512;
    if (x >= 240 || y >= 160 || x + w <= 0 || y + h <= 0) return null;
    return [x, y, w, h];
  }
  function attributionOn() {
    if (att.ids.length || !oam.count || !oam.pcount) return;
    for (const fn of EMITTERS) {
      if (!(fn in g.types.functions)) continue;
      att.ids.push(g.on(fn, (a) => {
        mem();
        att.stack.push({ obj: a[0] >>> 0, o: u32(oam.count) | 0, p: u32(oam.pcount) | 0 });
      }));
      att.ids.push(g.on(fn, () => {
        const s = att.stack.pop();
        if (!s) return;
        mem();
        const o = Math.min(128, u32(oam.count) | 0), p = Math.min(128, u32(oam.pcount) | 0);
        let boxes = null;
        for (let i = s.o; i < o; i++) { const b = boxOf(u32(oam.buf + 8 * i)); if (b) (boxes || (boxes = [])).push(b); }
        for (let i = s.p; i < p; i++) { const b = boxOf(u32(oam.pbuf + 8 * i)); if (b) (boxes || (boxes = [])).push(b); }
        if (!boxes) return;
        const list = att.frame.get(s.obj);
        if (list) list.push(...boxes); else att.frame.set(s.obj, boxes);
      }, { exit: true }));
    }
  }
  // Call at each frame boundary while attribution is wanted: the objects drawn
  // last frame, grouped by the task that owns them.
  function attributionFrame() {
    attributionOn();
    const byObj = att.frame;
    att.frame = new Map();
    att.stack.length = 0;
    const tasks = (g.lib.tasks ? g.lib.tasks() : []).filter((t) => !t.dead).map((t) => [Number(t.task), t.update]).sort((x, y) => x[0] - y[0]);
    const byTask = new Map();
    for (const [obj, boxes] of byObj) {
      let owner = null;
      for (const t of tasks) { if (t[0] <= obj && obj - t[0] < 0x2000) owner = t; else if (t[0] > obj) break; }
      if (!owner) continue;
      let e = byTask.get(owner[0]);
      if (!e) byTask.set(owner[0], e = { task: owner[0], update: owner[1], boxes: [] });
      e.boxes.push(...boxes);
    }
    att.last = byTask;
  }
  function attributionOff() {
    att.ids.forEach((id) => g.off(id));
    att.ids = []; att.last = new Map(); att.frame = new Map(); att.stack.length = 0;
  }
  const union = (boxes) => {
    const x0 = Math.min(...boxes.map((b) => b[0])), y0 = Math.min(...boxes.map((b) => b[1]));
    const x1 = Math.max(...boxes.map((b) => b[0] + b[2])), y1 = Math.max(...boxes.map((b) => b[1] + b[3]));
    return [x0, y0, x1 - x0, y1 - y0];
  };
  // Inspect boxes: each task's sprites this frame (union), for hover and click.
  function owned() {
    const out = [];
    for (const e of att.last.values()) {
      const [x, y, w, h] = union(e.boxes);
      if (w * h > 240 * 160 * 0.8) continue; // a full-screen layer, not an actor
      out.push({ addr: g.hex(e.task), task: g.hex(e.task), owner: e.update, x, y, w, h, pieces: e.boxes.length });
    }
    return out;
  }

  // ---- world position of a typed object ----------------------------------------
  const posField = new Map();
  function positionOf(task, type) {
    if (!type) return null;
    if (!posField.has(type)) {
      const r = g.resolve(type);
      const f = flatFields(r).find((x) => /^(position|pos)$/i.test(x.name));
      let ok = null;
      if (f) {
        const fr = g.resolve(f.type);
        const names = new Map(flatFields(fr).map((x) => [x.name, x]));
        if (names.has('x') && names.has('y') && names.has('z')) ok = { off: f.off, x: names.get('x'), y: names.get('y'), z: names.get('z') };
      }
      posField.set(type, ok);
    }
    const p = posField.get(type);
    if (!p) return null;
    return ['x', 'y', 'z'].map((k) => scalar(task + p.off + p[k].off, p[k].type, p[k]).v);
  }

  // ---- scene: actors in world space -------------------------------------------
  function scene() {
    const out = [];
    const params = g.types.params || {};
    for (const e of att.last.values()) {
      const type = params[e.update];
      const world = positionOf(e.task, type);
      if (!world || (!world[0] && !world[1] && !world[2])) continue;
      const box = union(e.boxes);
      if (box[2] * box[3] > 240 * 160 * 0.5) continue;
      const p = g.lib.project(world[0], world[1], world[2]);
      out.push({ addr: e.task, owner: e.update, world, box, anchor: [p.x, p.y] });
    }
    const pl = g.$.gPlayers && g.$.gPlayers[0];
    const player = pl ? [pl.position.x, pl.position.y, pl.position.z] : null;
    const cam = g.types.globals.gCameraOrigin;
    const camera = cam ? [0, 1, 2].map((i) => g.s16(cam.addr + 2 * i)) : null;
    return { sprites: out, player, camera };
  }

  // The followed actor: a task (its typed object) and the sprites it drew.
  function info(sel) {
    const tasks = g.lib.tasks ? g.lib.tasks() : [];
    const t = tasks.find((x) => Number(x.task) === sel.task);
    const out = { addr: sel.task, alive: !!(t && !t.dead), frame: g.frameNo };
    if (!t) return out;
    const type = (g.types.params || {})[t.update] || null;
    out.owner = { task: sel.task, update: t.update, type, id: t.id, list: t.list };
    const e = att.last.get(sel.task);
    if (e && e.boxes.length) { out.box = union(e.boxes); out.pieces = e.boxes.length; }
    out.hidden = !out.box;
    out.world = positionOf(sel.task, type);
    if (type) {
      out.size = g.sizeof(type);
      const budget = { n: 1500, enums: {} };
      out.fields = tree(sel.task, type, 3, budget);
      out.enums = budget.enums; // enum type name -> {NAME: value}, for menus
    } else {
      out.raw = g.hex(sel.task, 64);
    }
    return out;
  }

  // Set a field by path (names and indices) under the actor's struct.
  function set(base, type, path, value) {
    let v = g.at(base, type);
    for (const k of path.slice(0, -1)) v = v[k];
    v[path[path.length - 1]] = value;
    return true;
  }

  return { info, scene, spriteLayer, set, ownerOf, owned, attributionFrame, attributionOff };
}
