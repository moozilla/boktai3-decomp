// Skills every script sees by name (port/agent/README.md, "Library"). A file
// here is a plain script: `g`, `$` and `print` are in scope, and what it puts
// on `exports` becomes a global of every script. Functions that run frames are
// generators: call them with `yield*` (or just `yield`).

const GAME_STATE = 0x02000710;

// Scene, player position, life: the one-line state.
exports.where = () => {
  const gs = g.u32(GAME_STATE);
  const p = $.gPlayers[0];
  const out = { frame: g.frameNo };
  if (gs >= 0x02000000 && gs < 0x02040000) {
    out.scene = g.u32(gs + 0x5a4);
    out.life = g.u16(gs + 0x28);
  }
  if (p) Object.assign(out, { x: p.position.x, y: p.position.y, z: p.position.z });
  return out;
};

// Every scheduler task: list, address, id, update function.
exports.tasks = () => {
  const out = [];
  const lists = $.gTaskLists;
  for (let i = 0; i < 14; i++) {
    for (let t = lists[i].head, n = 0; t && n < 512; t = t.next, n++) {
      out.push({ list: i, task: g.hex(t.$addr), id: t.id, update: t.update ? g.name(t.update) : '-', dead: !!(t.flags & 1) });
    }
  }
  return out;
};

// The task whose allocation holds ADDR (the nearest task header below it).
exports.taskOf = (addr) => {
  let best = null;
  for (const t of exports.tasks()) {
    const a = Number(t.task);
    if (a <= addr && addr - a < 0x2000 && (!best || a > Number(best.task))) best = t;
  }
  return best;
};

const OBJ_SIZE = [[[8, 8], [16, 16], [32, 32], [64, 64]], [[16, 8], [32, 8], [32, 16], [64, 32]], [[8, 16], [8, 32], [16, 32], [32, 64]]];
function oamBox(e) {
  const a0 = e.attributes & 0xffff, a1 = e.attributes >>> 16;
  if ((a0 & 0x300) === 0x200) return null; // hidden
  const shape = a0 >> 14, size = a1 >> 14;
  let [w, h] = (OBJ_SIZE[shape] || OBJ_SIZE[0])[size];
  if (a0 & 0x200) { w *= 2; h *= 2; } // affine double size
  let y = a0 & 0xff, x = a1 & 0x1ff;
  if (y >= 160) y -= 256;
  if (x >= 240) x -= 512;
  return { x, y, w, h };
}

// What drew each sprite this frame: runs one frame with Sprite_QueueFramePieces
// hooked and returns [{sprite, x, y, w, h, pieces, owner}] in screen pixels.
exports.sprites = function* () {
  const out = [], stack = [];
  const enter = g.on('Sprite_QueueFramePieces', (a) => {
    stack.push({ sprite: a[0], o: $.gOamCount, p: $.gPendingOamCount });
  });
  const exit = g.on('Sprite_QueueFramePieces', () => {
    const s = stack.pop();
    if (!s) return;
    const boxes = [];
    for (let i = s.o; i < $.gOamCount; i++) boxes.push(oamBox($.gOamBuffer[i]));
    for (let i = s.p; i < $.gPendingOamCount; i++) boxes.push(oamBox($.gPendingOamBuffer[i]));
    const vis = boxes.filter(Boolean);
    if (!vis.length) return;
    const x0 = Math.min(...vis.map((b) => b.x)), y0 = Math.min(...vis.map((b) => b.y));
    const x1 = Math.max(...vis.map((b) => b.x + b.w)), y1 = Math.max(...vis.map((b) => b.y + b.h));
    out.push({ sprite: s.sprite, x: x0, y: y0, w: x1 - x0, h: y1 - y0, pieces: vis.length });
  }, { exit: true });
  yield 1;
  g.off(enter); g.off(exit);
  const tasks = exports.tasks();
  for (const s of out) {
    let best = null;
    for (const t of tasks) {
      const a = Number(t.task);
      if (a <= s.sprite && s.sprite - a < 0x2000 && (!best || a > Number(best.task))) best = t;
    }
    s.owner = best ? best.update : '?';
    s.task = best ? best.task : null;
    s.sprite = g.hex(s.sprite);
  }
  return out;
};

// The sprites under screen pixel (x, y), topmost first.
exports.spritesAt = function* (x, y) {
  const all = yield* exports.sprites();
  return all.filter((s) => x >= s.x && x < s.x + s.w && y >= s.y && y < s.y + s.h).reverse();
};

// Message window state, kept by hooks installed on first use:
// {open, id (string id of the page shown), opened (frame), pages}.
const message = { open: false, id: null, pages: 0, opened: null };
let messageHooks = null;
exports.message = () => {
  if (!messageHooks) {
    messageHooks = [
      g.on('MessageWindow_ShowMessage', () => { message.open = true; message.opened = g.frameNo; message.pages = 0; }),
      g.on('Text_LookupString', (a) => { message.id = a[0]; message.pages++; }),
      g.on('MessageWindow_Clear', () => { message.open = false; }),
    ];
  }
  return message;
};

// Press A until the message window closes (or MAX frames).
exports.advanceText = function* (max = 1800) {
  const m = exports.message();
  const start = g.frameNo;
  if (!m.open) yield g.until(() => m.open, 60);
  while (m.open && g.frameNo - start < max) {
    yield g.tap('a');
    yield 4;
  }
  return !m.open;
};

// Walk the player to world (x, z) with the d-pad, the way a player would.
// Learns which way each direction moves on this screen, sidesteps when stuck.
// Returns {reached, x, z, frames, reason}; gives up when 3 s pass without
// getting closer (opts.patience frames). It has no map: walls stop it.
exports.walkTo = function* (x, z, opts = {}) {
  const tolerance = opts.tolerance ?? 8, budget = opts.frames ?? 1200;
  // The field view is isometric: Down walks +x+z, Down+Right walks +x (measured).
  const dirs = {
    down: [0.7, 0.7], up: [-0.7, -0.7], left: [-0.7, 0.7], right: [0.7, -0.7],
    'down+right': [1, 0], 'up+left': [-1, 0], 'down+left': [0, 1], 'up+right': [0, -1],
  };
  const learned = Object.fromEntries(Object.entries(dirs).map(([k, v]) => [k, v.slice()]));
  if (!$.gPlayers[0]) yield g.until(() => $.gPlayers[0], 600);
  const p = $.gPlayers[0];
  if (!p) return { reached: false, reason: 'no player' };
  const start = g.frameNo, scene = exports.where().scene;
  let speed = 3, stuck = 0, side = 1, closest = Infinity, closestAt = g.frameNo;
  const result = (reached, reason) => ({ reached, x: p.position.x, z: p.position.z, frames: g.frameNo - start, reason });
  while (g.frameNo - start < budget) {
    const px = p.position.x, pz = p.position.z;
    const dx = x - px, dz = z - pz, dist = Math.hypot(dx, dz);
    if (dist <= tolerance) return result(true, 'arrived');
    if (dist < closest - 4) { closest = dist; closestAt = g.frameNo; }
    else if (g.frameNo - closestAt > (opts.patience ?? 180)) return result(false, `blocked ${Math.round(closest)} away`);
    if (exports.where().scene !== scene) return result(false, 'scene changed');
    let key = null, best = -2;
    for (const [k, v] of Object.entries(learned)) {
      const dot = (v[0] * dx + v[1] * dz) / dist;
      if (dot > best) { best = dot; key = k; }
    }
    if (stuck >= 2) {
      // sidestep: the direction most perpendicular to the goal, alternating sides
      let perp = null, bestPerp = 2;
      for (const [k, v] of Object.entries(learned)) {
        const dot = (v[0] * dx + v[1] * dz) / dist, cross = (v[0] * dz - v[1] * dx) / dist;
        if (Math.sign(cross) === side && Math.abs(dot) < bestPerp) { bestPerp = Math.abs(dot); perp = k; }
      }
      yield g.press(perp || key, 16);
      side = -side;
      if (stuck > 8) return result(false, 'stuck');
    }
    const frames = Math.max(2, Math.min(12, Math.round(dist / Math.max(speed, 1))));
    yield g.press(key, frames);
    const mx = p.position.x - px, mz = p.position.z - pz, moved = Math.hypot(mx, mz);
    if (moved < 1) { stuck++; continue; }
    stuck = 0;
    speed = moved / frames;
    const v = learned[key];
    v[0] = 0.5 * v[0] + 0.5 * mx / moved;
    v[1] = 0.5 * v[1] + 0.5 * mz / moved;
    const n = Math.hypot(v[0], v[1]) || 1;
    v[0] /= n; v[1] /= n;
  }
  return result(false, 'out of frames');
};

// From power-on with a save: title, file select, Continue (about 1700 frames;
// the same inputs as `play.py continue-save`).
exports.continueSave = function* () {
  for (const [keys, frames] of [[0, 950], ['a', 6], [0, 144], ['start', 6], [0, 144], ['a', 6], [0, 144], ['a', 6], [0, 300]]) {
    yield g.press(keys, frames);
  }
  return exports.where();
};

// World <-> screen, as SpriteOam_ProjectPosition draws projected sprites.
const proj = (v) => { v *= 3; return v >= 0 ? v >> 5 : -((-v) >> 5); };
const camera = () => [0, 1, 2].map((i) => g.s16(g.types.globals.gCameraOrigin.addr + 2 * i));
exports.project = (x, y, z) => {
  const [cx, cy] = camera(), depth = proj(x + z), height = proj(y);
  return { x: proj(x - z) - cx + 120, y: depth - height - cy + 80 };
};
// The world point under screen pixel (sx, sy) at height y (default: the player's).
exports.unproject = (sx, sy, y) => {
  if (y === undefined) y = $.gPlayers[0] ? $.gPlayers[0].position.y : 0;
  const [cx, cy] = camera();
  const a = sx - 120 + cx, b = sy - 80 + cy + proj(y);
  return { x: Math.round((a + b) * 16 / 3), y, z: Math.round((b - a) * 16 / 3) };
};

// Every projected sprite in the current render list with its screen box and
// the task that owns its memory: [{addr, x, y, w, h, world, owner, task}].
exports.drawables = () => {
  const heads = g.types.globals.gDrawableListHeads;
  if (!heads) return [];
  const head = g.u32(heads.addr + 4 * ($.gRenderListIndex & 1));
  const tasks = exports.tasks();
  const out = [];
  for (let d = head ? g.at(head, 'struct SpriteDrawable') : null, n = 0; d && n < 256; d = d.next, n++) {
    const p = d.position;
    let x = p.x, y = p.y;
    if (!(d.flags & 16)) ({ x, y } = exports.project(p.x, p.y, p.z));
    const box = { addr: g.hex(d.$addr), x: x + d.offsetX, y: y + d.offsetY, w: d.width, h: d.height,
      world: d.flags & 16 ? null : [p.x, p.y, p.z], hidden: !!(d.flags & 0x401) };
    let best = null;
    for (const t of tasks) {
      const a = Number(t.task);
      if (a <= d.$addr && d.$addr - a < 0x2000 && (!best || a > Number(best.task))) best = t;
    }
    box.owner = best ? best.update : '?';
    box.task = best ? best.task : null;
    out.push(box);
  }
  return out;
};
