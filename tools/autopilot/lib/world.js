// World model and navigation for the autopilot (agent library script).
//
// Reads the live terrain the game itself uses (tools/assets/hp.py documents the
// format): gTerrainMap (struct EngineMap) -> cells, a grid header (u32 0, u16 width,
// u16 height, s16 origin x, y) followed by struct SceneCell[width * height] at +12;
// the cell under world (x, z) is rows[z >> 8] + (x >> 8) (ActorPosition_CellIndex).
// Cell flags (tentative): 0x0400 marks walls (0x040B), 0x0003 outside the stage;
// 0x000A is a slope end, which is walkable although bit 3 is set. Boxes (doors, areas,
// entrances) are gTerrainMap->boxes: u32 count, then {u16 x0, z0, x1, z1; u8 y0, y1;
// u16 name} in world units (y in 16ths of a level).

const CELL = 256;

exports.grid = () => {
  const map = $.gTerrainMap;
  if (!map) return null;
  const base = Number(map.cells.$addr ?? map.cells), w = g.u16(base + 4), h = g.u16(base + 6);
  const cells = [];
  for (let z = 0; z < h; z++) {
    const row = map.rows[z];
    for (let x = 0; x < w; x++) {
      const a = base + 12 + 4 * (row + x), hb = g.u8(a), flags = g.u16(a + 2);
      cells.push({ x, z, level: hb & 15, slope: hb >> 4 === 1, flags, open: !(flags & 0x400) && (flags & 3) !== 3 });
    }
  }
  return { w, h, cells, at: (x, z) => (x < 0 || z < 0 || x >= w || z >= h ? null : cells[z * w + x]) };
};

exports.boxes = () => {
  const map = $.gTerrainMap;
  if (!map) return [];
  const base = Number(map.boxes.$addr ?? map.boxes), n = g.u32(base), out = [];
  for (let i = 0; i < n && i < 256; i++) {
    const b = base + 4 + 12 * i;
    out.push({ name: g.u16(b + 10).toString(16).toUpperCase().padStart(4, '0'), x0: g.u16(b), z0: g.u16(b + 2),
      x1: g.u16(b + 4), z1: g.u16(b + 6), y0: g.u8(b + 8) * 16, y1: g.u8(b + 9) * 16 });
  }
  return out;
};

exports.cellOf = (x, z) => [x >> 8, z >> 8];

// A* over open cells (8 neighbors; a diagonal needs both sides open). goals: a set
// of "x,z" keys. blocked: extra "x,z" keys learned from failed moves. Returns
// [[x, z], ...] from start to a goal, or null.
exports.findPath = (grid, start, goals, blocked = new Set()) => {
  const key = (x, z) => x + ',' + z;
  const open = (x, z) => { const c = grid.at(x, z); return c && c.open && !blocked.has(key(x, z)); };
  const gx = [...goals].map((k) => k.split(',').map(Number));
  const hfn = (x, z) => Math.min(...gx.map(([a, b]) => Math.max(Math.abs(a - x), Math.abs(b - z))));
  const came = new Map(), cost = new Map([[key(...start), 0]]), frontier = [[hfn(...start), ...start]];
  while (frontier.length) {
    frontier.sort((a, b) => a[0] - b[0]);
    const [, x, z] = frontier.shift(), k = key(x, z);
    if (goals.has(k)) {
      const path = [[x, z]];
      for (let c = k; came.has(c);) { c = came.get(c); path.unshift(c.split(',').map(Number)); }
      return path;
    }
    for (const [dx, dz] of [[1, 0], [-1, 0], [0, 1], [0, -1], [1, 1], [1, -1], [-1, 1], [-1, -1]]) {
      const nx = x + dx, nz = z + dz;
      if (!open(nx, nz) && !goals.has(key(nx, nz))) continue;
      if (dx && dz && !(open(x + dx, z) && open(x, z + dz))) continue;
      const nk = key(nx, nz), c = cost.get(k) + (dx && dz ? 1.414 : 1);
      if (c < (cost.get(nk) ?? Infinity)) { cost.set(nk, c); came.set(nk, k); frontier.push([c + hfn(nx, nz), nx, nz]); }
    }
  }
  return null;
};

// Drop waypoints that lie on a straight line between their neighbors.
function simplify(path) {
  if (path.length < 3) return path;
  const out = [path[0]];
  for (let i = 1; i < path.length - 1; i++) {
    const [a, b, c] = [out[out.length - 1], path[i], path[i + 1]];
    const d1 = [Math.sign(b[0] - a[0]), Math.sign(b[1] - a[1])], d2 = [Math.sign(c[0] - b[0]), Math.sign(c[1] - b[1])];
    if (d1[0] !== d2[0] || d1[1] !== d2[1]) out.push(b);
  }
  out.push(path[path.length - 1]);
  return out;
}

// Read any message the game opened (A until it closes) and drive any menu that came
// up; true when there was something.
exports.clearInterruptions = function* (opts = {}) {
  let any = false;
  opts.seen = opts.seen || new Map();
  for (let i = 0; i < 20; i++) {
    const m = exports.message();
    if (m.open) {
      const id = m.id;
      yield* exports.advanceText(900);
      opts.seen.set(id, (opts.seen.get(id) || 0) + 1);
      if (opts.trace) opts.trace({ dialogue: id });
      any = true;
      continue;
    }
    const step = exports.menuStep ? yield* exports.menuStep(opts.policy || {}) : null;
    if (step) { if (opts.trace) opts.trace({ menu: step.menu }); any = true; continue; }
    break;
  }
  return any;
};

// Walk to box NAME (a door: entering it starts the transition) or to world (x, z).
// Plans on the live grid, follows cell centers with walkTo, marks cells it could
// not enter and re-plans. Returns { reached, reason, scene, frames, plans }.
exports.navigate = function* (target, opts = {}) {
  const start = g.frameNo, scene0 = exports.where().scene, blocked = new Set(), budget = opts.frames ?? 3600;
  let plans = 0, pushes = 0;
  exports.message();  // installs the message window hooks before anything opens
  const done = (reached, reason, extra) => ({ reached, reason, scene: exports.where().scene, frames: g.frameNo - start, plans, ...extra });
  while (g.frameNo - start < budget) {
    if (exports.where().scene !== scene0) return done(true, 'scene changed');
    const grid = exports.grid(), p = $.gPlayers[0];
    if (!grid || !p) { yield 1; continue; }
    let goals = new Set(), aim;
    if (typeof target === 'string') {
      const box = exports.boxes().find((b) => b.name === target.toUpperCase());
      if (!box) return done(false, `no box ${target}`);
      aim = [(box.x0 + box.x1) >> 1, (box.z0 + box.z1) >> 1];
      // the box's open cells, else the open cells next to it (a door in a wall)
      const inside = [];
      for (let cz = box.z0 >> 8; cz <= (box.z1 - 1) >> 8; cz++) for (let cx = box.x0 >> 8; cx <= (box.x1 - 1) >> 8; cx++) inside.push([cx, cz]);
      for (const [cx, cz] of inside) { const c = grid.at(cx, cz); if (c && c.open) goals.add(`${cx},${cz}`); }
      if (!goals.size) {
        for (const [cx, cz] of inside) for (const [dx, dz] of [[1, 0], [-1, 0], [0, 1], [0, -1]]) {
          const c = grid.at(cx + dx, cz + dz);
          if (c && c.open) goals.add(`${cx + dx},${cz + dz}`);
        }
      }
    } else {
      aim = target;
      goals.add(exports.cellOf(...target).join(','));
    }
    // the same lines again: a scripted gate turned the player back
    if (opts.seen && [...opts.seen.values()].some((n) => n >= 2)) return done(false, 'gated', { lines: [...opts.seen.keys()] });
    const here = exports.cellOf(p.position.x, p.position.z);
    const path = goals.has(here.join(',')) ? [here] : exports.findPath(grid, here, goals, blocked);
    plans++;
    if (opts.trace) opts.trace({ plan: plans, here, path: path && simplify(path), blocked: [...blocked], x: p.position.x, z: p.position.z });
    if (!path) return done(false, 'no path');
    let interrupted = false;
    for (const [cx, cz] of simplify(path).slice(1)) {
      const r = yield* exports.walkTo(cx * CELL + 128, cz * CELL + 128, { tolerance: 40, patience: 90 });
      if (exports.where().scene !== scene0) return done(true, 'scene changed');
      if (opts.trace) opts.trace({ to: [cx, cz], ...r });
      // a trigger's dialogue stops the player: read it, then re-plan from there
      if (yield* exports.clearInterruptions(opts)) { interrupted = true; break; }
      if (!r.reached) {
        // a scripted event can hold the player and talk only later
        yield g.until(() => exports.message().open, opts.eventWait ?? 600);
        if (yield* exports.clearInterruptions(opts)) { interrupted = true; break; }
        blocked.add(`${cx},${cz}`);
        break;
      }
    }
    if (interrupted) continue;
    // at the goal: push into the box's middle (a door triggers on contact)
    const here2 = exports.cellOf(p.position.x, p.position.z);
    if (goals.has(here2.join(','))) {
      const r = yield* exports.walkTo(aim[0], aim[1], { tolerance: 8, patience: 120 });
      if (exports.where().scene !== scene0) return done(true, 'scene changed');
      if (yield* exports.clearInterruptions(opts)) continue;
      if (typeof target !== 'string' && r.reached) return done(true, 'arrived');
      yield g.until(() => exports.where().scene !== scene0 || exports.message().open, opts.eventWait ?? 600);
      if (exports.where().scene !== scene0) return done(true, 'scene changed');
      if (yield* exports.clearInterruptions(opts)) continue;
      if (++pushes > 3) return done(false, 'at the box, no transition');
    }
  }
  return done(false, 'out of frames');
};

// Go next to world (x, z) (an open cell beside it, the nearest by path), face it and
// press A, then read whatever that started. Returns { done, events } where events
// counts dialogue lines and menus that followed.
exports.interact = function* (x, z, opts = {}) {
  exports.message();
  const grid = exports.grid(), p = $.gPlayers[0];
  if (!grid || !p) return { done: false, reason: 'no field' };
  const [tx, tz] = exports.cellOf(x, z), here = exports.cellOf(p.position.x, p.position.z);
  const goals = new Set();
  for (const [dx, dz] of [[0, 1], [1, 0], [-1, 0], [0, -1]]) { const c = grid.at(tx + dx, tz + dz); if (c && c.open) goals.add(`${tx + dx},${tz + dz}`); }
  const path = goals.has(here.join(',')) ? [here] : exports.findPath(grid, here, goals);
  if (!path) return { done: false, reason: 'no path' };
  const [sx, sz] = path[path.length - 1];
  const r = yield* exports.navigate([sx * 256 + 128, sz * 256 + 128], opts);
  if (!r.reached) return { done: false, reason: 'could not get there: ' + r.reason };
  yield* exports.walkTo(x, z, { tolerance: 8, patience: 20, frames: 60 });  // face it
  const seen = new Map();
  yield g.tap('a');
  yield g.until(() => exports.message().open, 90);
  const any = yield* exports.clearInterruptions({ ...opts, seen });
  return { done: any, lines: [...seen.keys()] };
};
