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
  if (!map || !map.cells) return null;
  const base = Number(map.cells.$addr ?? map.cells), w = g.u16(base + 4), h = g.u16(base + 6);
  const cells = [];
  for (let z = 0; z < h; z++) {
    const row = map.rows[z];
    for (let x = 0; x < w; x++) {
      // getCell (scene_object.c): a terrain modifier's override wins over the map
      // (pushed blocks, raised bridges)
      const over = g.fn.TerrainModifiers_FindCellOverride(row + x, 1);
      const a = over ? over + 4 : base + 12 + 4 * (row + x), hb = g.u8(a), flags = g.u16(a + 2);
      cells.push({ x, z, index: row + x, level: hb & 15, slope: hb >> 4 !== 0, kind: hb >> 4, flags, open: !(flags & 1) });  // Player_IsTurnTerrainBlocked: bit 0 blocks (walls 0x040B/0x040F, outside 0x0003)
    }
  }
  return { w, h, cells, at: (x, z) => (x < 0 || z < 0 || x >= w || z >= h ? null : cells[z * w + x]) };
};

// Scene objects (src/objects/scene_objects/scene_object.c, gSceneObjectManager->objects): blocks and
// crates. An idle one (kind 0) can be pushed (SceneObjects_TryPushNeighbor) from a
// cell one level below its top; type 1 movable block, 5 limited pushes.
exports.blocks = () => {
  const mgr = $.gSceneObjectManager;
  if (!mgr) return [];
  const T = g.types.types, sz = T['struct SceneObject'].size;
  const base = g.u32(mgr.$addr + T['struct SceneObjectManager'].fields.find((f) => f.name === 'objects').off);
  const w = exports.grid().w, out = [];
  for (let i = 0; i < mgr.count; i++) {
    const o = g.at(base + i * sz, 'struct SceneObject');
    if (!o.active || o.mode === 2 || o.mode === 3) continue;
    const idx = o.cellRef.index;
    out.push({ slot: i, type: o.type, kind: o.kind, x: idx % w, z: Math.floor(idx / w), top: o.cellRef.cell.height & 15,
      health: o.health, pushes: o.pushLimited ? o.pushesLeft : Infinity, view: o });
  }
  return out;
};

exports.boxes = () => {
  const map = $.gTerrainMap;
  if (!map || !map.boxes) return [];
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
exports.findPath = (grid, start, goals, blocked = new Set(), loose = false) => {
  const key = (x, z) => x + ',' + z;
  const open = (x, z) => { const c = grid.at(x, z); return c && c.open && !blocked.has(key(x, z)); };
  // a step between cells: the same level, or through a slope at most one level apart
  // (the player cannot walk off an edge: the game stops it there, measured in 2776)
  // a slope only connects along its axis: kind 1 runs along z, kind 2 along x (2993's
  // stair (2,4)-(2,6) is kind 1; 2724's ramp (5,5)-(7,5) kind 2); off its side is a drop
  const along = (c, dx, dz) => !c.slope || (c.kind === 1 ? dx === 0 : c.kind === 2 ? dz === 0 : true);
  const step = (a, b, dx = 0, dz = 0) => a.level === b.level ||
    ((a.slope || b.slope) && Math.abs(a.level - b.level) <= 1 && along(a, dx, dz) && along(b, dx, dz));
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
      const from = grid.at(x, z), to = grid.at(nx, nz);
      if (to && to.open && !step(from, to, dx, dz)) continue;
      // a diagonal needs both sides open; loose: one side is enough (squeezing past a
      // person or a piece of furniture that blocks the other)
      const side = (cx, cz) => open(cx, cz) && step(from, grid.at(cx, cz), cx - x, cz - z);
      if (dx && dz && !(loose ? side(x + dx, z) || side(x, z + dz) : side(x + dx, z) && side(x, z + dz))) continue;
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

// Walk into the neighboring cell (cx, cz) and to its middle, with the grid-axis keys
// (an isometric screen: up+right is -z, down+right +x, down+left +z, up+left -x;
// the diagonals are the plain directions), correcting sideways drift so narrow
// bridges are safe. Returns { reached, reason }.
const KEY8 = { '0,-1': 'up+right', '1,0': 'down+right', '0,1': 'down+left', '-1,0': 'up+left',
  '1,1': 'down', '-1,-1': 'up', '1,-1': 'right', '-1,1': 'left' };
exports.stepTo = function* (cx, cz, opts = {}) {
  const p = $.gPlayers[0], tx = cx * CELL + 128, tz = cz * CELL + 128, scene = exports.where().scene;
  let best = Infinity, since = 0;
  for (let t = 0; t < (opts.frames ?? 240); t += 2) {
    const dx = tx - p.position.x, dz = tz - p.position.z, dist = Math.max(Math.abs(dx), Math.abs(dz));
    if (dist <= (opts.tolerance ?? 40)) return { reached: true, x: p.position.x, z: p.position.z };
    if (exports.where().scene !== scene) return { reached: false, reason: 'scene changed' };
    if (dist < best - 2) { best = dist; since = 0; } else if ((since += 2) > (opts.patience ?? 60)) return { reached: false, reason: `blocked ${best} away`, x: p.position.x, z: p.position.z };
    // move on each axis whose offset is big enough; small offsets are left alone
    const sx = Math.abs(dx) > 24 ? Math.sign(dx) : 0, sz = Math.abs(dz) > 24 ? Math.sign(dz) : 0;
    const key = KEY8[`${sx},${sz}`];
    if (!key) return { reached: true, x: p.position.x, z: p.position.z };
    yield g.press(key, 2);
  }
  return { reached: false, reason: 'out of frames' };
};

// stepTo with a fall guard: snapshot, step, and if the player dropped a level (or
// the cell is a narrow ledge, flags 0x800) fall back to searchTo. Needs
// exports.session (the autopilot sets it).
exports.safeStep = function* (cx, cz, opts = {}) {
  const S = exports.session, p = $.gPlayers[0], y0 = p.position.y, grid = exports.grid();
  const here = grid.at(p.position.x >> 8, p.position.z >> 8), to = grid.at(cx, cz);
  const narrow = (c) => c && (c.flags & 0x800);
  if (S && (narrow(to) || narrow(here))) return yield* exports.searchTo(cx, cz, opts);
  const snap = S ? S.snapshot() : null;
  const r = yield* exports.stepTo(cx, cz, opts);
  if (snap && p.position.y < y0 - 64 && !(to && to.level < here.level)) {
    S.restore(snap);
    if (opts.trace) opts.trace({ fell: [cx, cz], searching: true });
    return yield* exports.searchTo(cx, cz, opts);
  }
  return r;
};

// Snapshot beam search for inputs that bring the player into cell (cx, cz) without
// dropping height or leaving the scene: macro actions are the 8 directions held for
// 8 frames; score is the distance to the cell's middle. For moves the model does not
// cover (sidling along a ledge while clinging to the wall, ice, conveyors).
exports.searchTo = function* (cx, cz, opts = {}) {
  const S = exports.session, p = $.gPlayers[0], scene = exports.where().scene;
  const tx = cx * 256 + 128, tz = cz * 256 + 128, y0 = p.position.y, life0 = exports.where().life;
  const KEYS = ['up', 'down', 'left', 'right', 'up+left', 'up+right', 'down+left', 'down+right'];
  // distance to the cell (0 inside it): clinging to a wall keeps the player off the middle
  const dist = () => Math.max(Math.max(0, Math.abs(tx - p.position.x) - 127), Math.max(0, Math.abs(tz - p.position.z) - 127));
  let beam = [{ snap: S.snapshot(), d: dist(), keys: [] }];
  const seen = new Set();
  for (let depth = 0; depth < (opts.depth ?? 24); depth++) {
    const next = [];
    for (const node of beam) {
      for (const [k, n] of KEYS.flatMap((k) => [[k, 8], [k, 32]])) {
        S.restore(node.snap);
        yield g.press(k, n);
        const w = exports.where();
        if (w.scene !== scene || p.position.y < y0 - 64 || (w.life ?? 0) < (life0 ?? 0)) continue;
        // still standing a few frames later with the same key? (a step off an edge
        // looks fine on its first frame)
        { const look = S.snapshot(); yield g.press(k, 6); const fell = p.position.y < y0 - 64; S.restore(look); if (fell) continue; }
        const key = `${p.position.x >> 4},${p.position.z >> 4},${p.statusFlags & 0xff}`;
        if (seen.has(key)) continue;
        seen.add(key);
        const d = dist(), keys = [...node.keys, `${k}:${n}`];
        if (d === 0) { if (opts.trace) opts.trace({ searched: [cx, cz], keys: keys.join(' ') }); return { reached: true, x: p.position.x, z: p.position.z, searched: keys.length }; }
        next.push({ snap: S.snapshot(), d, keys, mode: p.statusFlags & 0xff });
      }
    }
    // the closest nodes, plus the closest two of each player mode (clinging, ...):
    // a mode change can be the way forward without getting closer at first
    next.sort((a, b) => a.d - b.d);
    const keep = new Set(next.slice(0, opts.beam ?? 6)), perMode = {};
    for (const n of next) if ((perMode[n.mode] = (perMode[n.mode] || 0) + 1) <= 2) keep.add(n);
    beam = [...keep];
    if (opts.debug) opts.debug({ depth, nodes: next.length, beam: beam.map((n) => `${n.d}/${n.mode}:${n.keys.slice(-2).join(',')}`) });
    if (!beam.length) break;
  }
  // no route: go back to where the search started
  S.restore(beam.length ? beam[0].snap : S.snapshot());
  return { reached: false, reason: 'search failed' };
};

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
  // never walk through another door's box on the way (opts.avoid: box names)
  const avoid = new Set();
  for (const b of exports.boxes()) if ((opts.avoid || []).includes(b.name) && b.name !== String(target).toUpperCase())
    for (let cz = b.z0 >> 8; cz <= (b.z1 - 1) >> 8; cz++) for (let cx = b.x0 >> 8; cx <= (b.x1 - 1) >> 8; cx++) avoid.add(`${cx},${cz}`);
  let plans = 0, pushes = 0;
  const tried = new Set();  // blocks a break attempt failed on
  const searched = new Set();  // cells a search already tried to get around an obstacle
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
    let path = goals.has(here.join(',')) ? [here] : exports.findPath(grid, here, goals, new Set([...blocked, ...avoid]));
    if (!path && blocked.size) path = exports.findPath(grid, here, goals, new Set([...blocked, ...avoid]), true);
    plans++;
    if (opts.trace) opts.trace({ plan: plans, here, path: path && simplify(path), blocked: [...blocked], x: p.position.x, z: p.position.z });
    if (!path && !opts.noPush) {
      const plan = exports.planPushes(grid, here, goals, exports.blocks());
      if (opts.trace) opts.trace({ pushPlan: plan && plan.map((p) => `${p.x},${p.z}>${p.dir}`) });
      if (plan && plan.length) {
        const r = yield* exports.push(plan[0], opts);
        if (opts.trace) opts.trace({ push: plan[0], ...r });
        if (r.done) { blocked.clear(); continue; }
      }
    }
    if (!path && !opts.noBreak) {
      // breakable blocks (wooden crates, health > 0) in the way: plan as if they were
      // gone, then break the first one on the route with the weapon
      const crates = exports.blocks().filter((b) => b.health > 0 && !tried.has(b.slot));
      const open = { ...grid, cells: grid.cells.slice(), at: null };
      for (const b of crates) { const c = grid.at(b.x, b.z); if (c) open.cells[c.index] = { ...c, level: b.top - 1, open: true }; }
      open.at = (x, z) => (x < 0 || z < 0 || x >= grid.w || z >= grid.h ? null : open.cells[z * grid.w + x]);
      const route = exports.findPath(open, here, goals, new Set([...blocked, ...avoid]));
      const first = route && route.map(([x, z]) => crates.find((b) => b.x === x && b.z === z)).find(Boolean);
      if (opts.trace) opts.trace({ breakPlan: first ? [first.x, first.z] : null });
      if (first) {
        tried.add(first.slot);
        const r = yield* exports.breakBlock(first, opts);
        if (opts.trace) opts.trace({ broke: [first.x, first.z], ...r });
        if (r.done) { blocked.clear(); tried.delete(first.slot); }
        continue;
      }
    }
    if (!path) return done(false, 'no path');
    let interrupted = false;
    for (const [cx, cz] of path.slice(1)) {
      if (opts.stopIf && opts.stopIf()) return done(false, 'stopped');
      const r = yield* exports.safeStep(cx, cz, opts);
      if (exports.where().scene !== scene0) return done(true, 'scene changed');
      if (opts.trace) opts.trace({ to: [cx, cz], ...r });
      // a trigger's dialogue stops the player: read it, then re-plan from there
      if (yield* exports.clearInterruptions(opts)) { interrupted = true; break; }
      if (!r.reached) {
        // a scripted event can hold the player and talk only later
        yield g.until(() => exports.message().open, opts.eventWait ?? 600);
        if (yield* exports.clearInterruptions(opts)) { interrupted = true; break; }
        // something the grid does not show (a person, furniture): search for inputs
        // that get into the cell anyway, once per cell
        const k = `${cx},${cz}`;
        if (exports.session && !searched.has(k)) {
          searched.add(k);
          const s2 = yield* exports.searchTo(cx, cz, opts);
          if (opts.trace) opts.trace({ around: [cx, cz], ...s2 });
          if (exports.where().scene !== scene0) return done(true, 'scene changed');
          if (s2.reached) continue;
        }
        blocked.add(k);
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
  const [tx, tz] = exports.cellOf(x, z);
  // next to it first; then across a counter (two cells away in a straight line, the cell
  // between blocked: a shopkeeper behind the counter)
  const near = new Set(), across = new Set();
  for (const [dx, dz] of [[0, 1], [1, 0], [-1, 0], [0, -1]]) {
    const c = grid.at(tx + dx, tz + dz), mid = c, far = grid.at(tx + 2 * dx, tz + 2 * dz);
    if (c && c.open) near.add(`${tx + dx},${tz + dz}`);
    else if (opts.across !== false && mid && far && far.open) across.add(`${tx + 2 * dx},${tz + 2 * dz}`);
  }
  let last = { done: false, reason: 'no path' };
  for (const goals of [near, across]) {
    const tried = new Set();
    for (let attempt = 0; attempt < 2 && goals.size > tried.size; attempt++) {
      const here = exports.cellOf(p.position.x, p.position.z);
      const left = new Set([...goals].filter((k) => !tried.has(k)));
      const path = left.has(here.join(',')) ? [here] : exports.findPath(grid, here, left);
      if (!path) break;
      const [sx, sz] = path[path.length - 1];
      tried.add(`${sx},${sz}`);
      const r = yield* exports.navigate([sx * 256 + 128, sz * 256 + 128], opts);
      if (!r.reached) { last = { done: false, reason: 'could not get there: ' + r.reason }; continue; }
      yield* exports.walkTo(x, z, { tolerance: 8, patience: 20, frames: 60 });  // face it
      const seen = new Map();
      yield g.tap('a');
      yield g.until(() => exports.message().open, 90);
      const any = yield* exports.clearInterruptions({ ...opts, seen });
      last = { done: any, lines: [...seen.keys()] };
      if (any) return last;
    }
  }
  return last;
};

// The player's bags (docs/ASSETS.md, Items): items GameState+0xA0 (u16 per slot),
// accessories +0x160 (u16), swords +0x1C0 (16-byte records, ID first), valuables +0x838,
// 0xFFFF / 0xFF mark empty slots.
exports.bags = () => {
  const gs = g.u32(g.types.globals.gGameState.addr);
  const read = (off, n, stride, size) => { const out = []; for (let i = 0; i < n; i++) { const v = size === 2 ? g.u16(gs + off + i * stride) : g.u8(gs + off + i * stride); if (v !== (size === 2 ? 0xffff : 0xff)) out.push(v); } return out; };
  // gun lenses +0x828 (u16 lens id = the gun's element mode, SolarFruit_GetInventorySlot),
  // gun frames +0x4C0 (index + 1 into gUnk_08E60F4C, 0 empty: GameState_GetUnlockEntry)
  const frames = []; for (let i = 0; i < 16; i++) { const v = g.u16(gs + 0x4c0 + i * 2); if (v) frames.push(v - 1); }
  return { items: read(0xa0, 16, 2, 2), accessories: read(0x160, 16, 2, 2), swords: read(0x1c0, 8, 16, 1),
    lenses: read(0x828, 8, 2, 2), frames };
};

// Pick up a floor item (stages.json proc_1118: it sits in the middle of its cell): walk
// onto it, then let the pickup message play out. Returns what the bags gained.
exports.collect = function* (x, z, opts = {}) {
  const before = JSON.stringify(exports.bags());
  // an item actor (proc_1119/1120, chest or not) opens with A from a neighboring cell;
  // a plain floor pickup is taken by walking onto it
  const i = yield* exports.interact(x, z, opts);
  if (JSON.stringify(exports.bags()) === before && i.reason !== 'no path') {
    yield* exports.walkTo(x, z, { tolerance: 16, patience: 30, frames: 90 });
    yield g.until(() => exports.message().open, 40);
    yield* exports.clearInterruptions(opts);
  }
  const after = exports.bags();
  return { done: JSON.stringify(after) !== before, lines: i.lines, reason: i.reason, bags: after };
};

// Attack the thing in cell (tx, tz) with the weapon: stand on a neighboring open cell at
// `level` (default: the target cell's floor), face it and attack (B) until done() holds
// or `hits` attacks. A crate (SceneObject with health), a lever switch (a hittable whose
// script runs when struck).
exports.hitAt = function* (tx, tz, opts = {}) {
  const grid = exports.grid(), p = $.gPlayers[0], level = opts.level ?? (grid.at(tx, tz) || {}).level;
  const goals = new Set();
  const dist = opts.dist ?? 1;
  for (const [dx, dz] of DIRS) {
    const c = grid.at(tx - dx * dist, tz - dz * dist);
    if (c && c.open && c.level === level) goals.add(`${tx - dx * dist},${tz - dz * dist}`);
  }
  const here = exports.cellOf(p.position.x, p.position.z);
  const path = goals.has(here.join(',')) ? [here] : exports.findPath(grid, here, goals);
  if (!path) return { done: false, reason: 'no cell next to it' };
  const [sx, sz] = path[path.length - 1];
  const r = yield* exports.navigate([sx * 256 + 128, sz * 256 + 128], { ...opts, noBreak: true, frames: 1800 });
  if (!r.reached) return { done: false, reason: 'could not get next to it: ' + r.reason };
  const key = exports.DIR_KEYS[DIRS.findIndex(([dx, dz]) => sx + dx * dist === tx && sz + dz * dist === tz)];
  const done = opts.done || (() => false);
  let hits = 0, lines = [];
  for (; hits < (opts.hits ?? 40) && !done(); hits++) {
    yield g.press(key, 4);  // face it
    yield g.press(key + '+b', 2);
    yield g.press(0, 16);
    const seen = new Map();
    if (yield* exports.clearInterruptions({ ...opts, seen })) { lines.push(...seen.keys()); if (opts.stopOnEvent) { hits++; break; } }
  }
  return { done: done(), hits, lines };
};

// The weapon in hand (src/player/stats/sub_081401A0.c Player_RefreshActionEquipment): GameState+0x50
// is the selected slot of the equip list at +0x68 (0-7 swords, 8-15 the Gun Del Sol with a
// frame); the gun's lens is lenses[slots88[4 + actionAuxiliarySlot]] (+0x828, +0x90, +0x54),
// whose id is the element mode Player_GetAttackAttributeFlags maps to reaction flags.
// Energy is the player's currentStatB (+0x428); a gun shot costs 3 with no frame special.
exports.weapon = () => {
  const gs = g.u32(g.types.globals.gGameState.addr), s16 = (o) => g.u16(gs + o) << 16 >> 16;
  const slot = s16(0x50), aux = s16(0x54), li = s16(0x90 + aux * 2);
  const p = $.gPlayers[0];
  return { slot, gun: slot > 7, lens: li >= 0 ? s16(0x828 + li * 2) : -1, energy: p ? p.currentStatB : 0, energyMax: p ? p.maximumStatB : 0 };
};

// Quick Change (Otenko 8759/8893: hold SELECT and press R to swap the sword and the gun,
// SELECT + B to swap the lens) until the weapon matches: { gun: bool, lens: id }.
exports.quickChange = function* (want) {
  for (let i = 0; i < 12; i++) {
    const w = exports.weapon();
    const key = want.gun !== undefined && w.gun !== want.gun ? 'r' : want.lens !== undefined && w.gun && w.lens !== want.lens ? 'b' : null;
    if (!key) return { done: true, ...w };
    yield g.press('select', 6);
    yield g.press('select+' + key, 4);
    yield g.press('select', 6);
    yield g.press(0, 20);
  }
  return { done: false, ...exports.weapon() };
};

// Wait for energy in sunlight (src/scene/area_effect/sub_0804E618.c: the area effect adds gGameState->unk876,
// the sun level, per frame; 120 make one energy). The light is the solar sensor (session
// setSolar: 0 bright .. 255 dark), written into the ROM script as `lux` lines.
exports.chargeEnergy = function* (need, opts = {}) {
  const session = exports.session, before = exports.weapon().energy;
  if (before >= need) return { done: true, energy: before };
  if (session && opts.lux !== undefined) session.setSolar(opts.lux);
  let t = 0;
  for (; t < (opts.frames ?? 3600) && exports.weapon().energy < need; t += 30) yield g.press(0, 30);
  if (session && opts.restore !== undefined) session.setSolar(opts.restore);
  return { done: exports.weapon().energy >= need, energy: exports.weapon().energy, before, frames: t };
};

// The pause menu (START) lives in the field HUD task (Hud_UpdateWithCallback,
// src/ui/hud/display/sub_08178B98.c): its state is the callback at +0xA50 (SceneHud_UpdatePageInput,
// PauseMenu_UpdateItemActions, ...), the page at +0x1414 (1 items, 2 valuables, ...), the
// grid cursor at +0x4384 (Inventory_InitializeSelectionGrid; +0x4387 the cell id).
exports.hudTask = () => { const t = exports.tasks().find((x) => !x.dead && x.update === 'Hud_UpdateWithCallback'); return t ? Number(t.task) : 0; };
exports.menuState = () => {
  const h = exports.hudTask();
  if (!h) return null;
  const cb = g.u32(h + 0xa50);
  // +0xA54: the page's own step (item chosen, use prompt, confirm), another callback
  const sub = g.u32(h + 0xa54);
  return { callback: cb ? g.name(cb) : null, sub: sub ? (g.name(sub) || sub.toString(16)) : null, page: g.u8(h + 0x1414), grid: [...g.bytes(h + 0x4384, 12)], extra: [g.u8(h + 0x43b3), g.u8(h + 0xa58)] };
};
// Reach a goal inside the pause menu by search: open it, breadth-first over menu keys from
// snapshots (a node = the menu state above), until goal() holds, then back out to the field.
// The menu's own state decides what each key did; nothing about the layout is assumed.
exports.menuSearch = function* (goal, opts = {}) {
  const S = exports.session, start = g.frameNo, fieldCb = exports.menuState() && exports.menuState().callback;
  const KEYS = opts.keys || ['down', 'right', 'up', 'left', 'a', 'r', 'l', 'b'];
  // the node: callback, its step (+0xA54), page, the live cursor (grid bytes 0-3) and the prompt flag; the
  // remembered cursors of other grids are left out (they multiply states, not choices)
  const keyOf = () => { const m = exports.menuState(); return m ? [m.callback, m.sub, m.page, ...m.grid.slice(0, 4), m.extra[1]].join(',') : ''; };
  yield g.press('start', 2); yield g.press(0, 60);
  // the menu opens on the page it was left on: L/R to the page asked for (1 items, 3 Solar Gun)
  for (let i = 0; i < 8 && opts.page !== undefined && exports.menuState() && exports.menuState().page !== opts.page; i++) {
    yield g.press(exports.menuState().page > opts.page ? 'l' : 'r', 2); yield g.press(0, 40);
  }
  let found = null, expanded = 0;
  const seen = new Set([keyOf()]), root = S.snapshot();
  // nodes are key paths replayed from the opened menu (a snapshot each would not fit)
  const wait = (k) => (['l', 'r', 'a', 'b'].includes(k) ? 40 : 14);
  const play = function* (path) { S.restore(root); for (const k of path) { yield g.press(k, 2); yield g.press(0, wait(k)); } };
  let queue = [[]];
  search: while (queue.length && expanded < (opts.max ?? 400)) {
    const node = queue.shift();
    expanded++;
    for (const k of KEYS) {
      const path = [...node, k];
      if (opts.tryTrace) opts.tryTrace(path.join(' '));
      yield* play(path);
      // an effect that plays out over frames (life refilling after an item is used):
      // after A, judge again once opts.settle more frames have run
      if (goal() || (k === 'a' && opts.settle && (yield g.press(0, opts.settle), goal()))) { found = path; break search; }
      const st = exports.menuState();
      if (!st || st.callback === fieldCb) continue;  // closed the menu
      const key = keyOf();
      if (seen.has(key)) continue;
      seen.add(key);
      if (opts.trace) opts.trace({ path: path.join(' '), state: st });
      queue.push(path);
    }
  }
  if (!found) yield* play([]);
  // back to the field: B until the HUD runs its field callback again
  for (let i = 0; i < 12 && exports.menuState() && exports.menuState().callback !== fieldCb; i++) { yield g.press('b', 2); yield g.press(0, 30); }
  if (exports.menuState() && exports.menuState().callback !== fieldCb) { yield g.press('start', 2); yield g.press(0, 40); }
  return { done: !!found, keys: found ? found.join(' ') : null, states: seen.size, expanded, frames: g.frameNo - start, closed: !exports.menuState() || exports.menuState().callback === fieldCb };
};

// Live Breakables (src/objects/breakable.c: gPooledInteractableManager slots): script id
// and state (1 unlit / 2 lit for a candlestick; Breakable_OnHit moves them on).
exports.breakables = () => {
  const m = $.gPooledInteractableManager;
  if (!m || !m.slots) return [];
  const first = typeof m.slots === 'object' ? m.slots : g.at(Number(m.slots), 'struct PooledInteractable'), out = [];
  for (let i = 0; i < m.count; i++) {
    const sl = first.$at(i);
    if (sl.active) out.push({ index: i, id: sl.id, state: sl.state, kind: sl.kind });
  }
  return out;
};

// Break a block (a wooden crate: SceneObject with health).
exports.breakBlock = function* (block, opts = {}) {
  const alive = () => exports.blocks().some((b) => b.slot === block.slot && b.health > 0);
  const r = yield* exports.hitAt(block.x, block.z, { ...opts, level: block.top - 1, done: () => !alive() });
  return { ...r, health: (exports.blocks().find((b) => b.slot === block.slot) || {}).health };
};

// A town map (src/ui/town/town_map.c, registry key 3F74): a cursor (src/ui/widgets/menu_cursor.c)
// moved freely with the D-pad over 11 entries; A on an entry runs its script (stages.json
// doors with "hub": index). hub() is the live menu or null.
exports.hub = () => {
  // a town map (Registry3F74, 11 entries) or San Miguel's gate (RegistrySelection,
  // src/ui/world_map.c, 9 entries with enabled flags)
  const t = exports.tasks().find((x) => !x.dead && (x.update === 'Registry3F74_OnUpdate' || x.update === 'RegistrySelection_OnUpdate'));
  if (!t) return null;
  const sel = t.update === 'RegistrySelection_OnUpdate';
  const menu = g.at(Number(t.task), sel ? 'struct RegistrySelectionMenu' : 'struct RegistryChoiceMenu');
  const ptr = sel ? menu.selectionCursor : menu.cursor;
  const cursor = ptr ? g.at(Number(ptr.$addr ?? ptr), 'struct MenuCursor') : null;
  // ready: the cursor reads input (src/ui/widgets/menu_cursor.c MenuCursor_UpdateInput)
  const ready = !!cursor && !!cursor.callback && g.name(cursor.callback) === 'MenuCursor_UpdateInput';
  return { menu, cursor, kind: sel ? '2388' : '3F74', ready, state: ready ? 'Registry3F74_UpdateSelection' : (menu.update ? g.name(menu.update) : '-') };
};

exports.chooseHub = function* (index, opts = {}) {
  const start = g.frameNo, scene = exports.where().scene;
  let presses = 0;
  for (let t = 0; t < (opts.frames ?? 1200); t++) {
    const h = exports.hub();
    if (!h || exports.where().scene !== scene) return { done: true, frames: g.frameNo - start };
    if (h.state !== 'Registry3F74_UpdateSelection' || !h.cursor) { yield 1; continue; }
    const c = h.cursor, e = c.entries[index], b = e.bounds;
    if (c.selectedIndex === index) {
      if (++presses > 3) return { done: false, reason: 'entry does not open' };
      yield g.press('a', 2);
      yield g.press(0, 4);
      yield g.until(() => !exports.hub() || exports.hub().state !== 'Registry3F74_UpdateSelection', 120);
      continue;
    }
    const tx = (b.left + b.right) >> 1, ty = (b.top + b.bottom) >> 1;
    const dx = tx - c.position.x, dy = ty - c.position.y;
    const keys = [dx < -1 ? 'left' : dx > 1 ? 'right' : null, dy < -1 ? 'up' : dy > 1 ? 'down' : null].filter(Boolean);
    yield g.press(keys.length ? keys.join('+') + (Math.max(Math.abs(dx), Math.abs(dy)) > 8 ? '+b' : '') : 0, 1);
  }
  return { done: false, reason: 'out of frames' };
};

// The story counter: r708[0x3c], r708 = gGameVariables (src/script/script_virtual_machine.c
// Script_DecodeVariableValue).
// San Miguel's facility flags r708[0x19a.0][i] (one bit each; proc 2566 needs ten of them).
exports.facilities = () => { const b = g.u32(g.types.globals.gGameVariables.addr); if (!b) return []; const v = g.u16(b + 0x19a) | (g.u8(b + 0x19c) << 16); const out = []; for (let i = 0; i < 16; i++) if (v & (1 << i)) out.push(i); return out; };
exports.story = () => { const b = g.u32(g.types.globals.gGameVariables.addr); return b ? g.u8(b + 0x3c) : null; };

// People in a town (src/npc/manager/scene_actor_lifecycle.c SceneActorManager: head, then each
// actor's previous; body.id is the character's id as place_character gives it).
exports.people = () => {
  const t = exports.tasks().find((x) => !x.dead && x.update === 'SceneActorManager_Update');
  if (!t) return [];
  const mgr = g.at(Number(t.task), 'struct SceneActorManager'), out = [];
  let a = mgr.head ? Number(mgr.head.$addr ?? mgr.head) : 0;
  for (let n = 0; a && n < 64; n++) {
    const v = g.at(a, 'struct SceneActor');
    if (v.active) out.push({ addr: a, id: v.body.id, x: v.body.position.x, z: v.body.position.z, enabled: v.enabled });
    a = v.previous ? Number(v.previous.$addr ?? v.previous) : 0;
  }
  return out;
};

// Push planning: a breadth-first search over block states with the game's rules
// (SceneObjects_TryPushNeighbor + Scene_TryStepDown). Heights include the blocks
// (a block raises its cell to its top). A push from P into direction d moves the
// block at B = P + d when level(P) + 1 == top, the cell T = B + d is flat, not
// flagged 8, and lower than the top: one level lower -> the block slides onto T;
// more -> it falls into T and fills it by one level. Returns the pushes
// [{x, z, dir, from: [px, pz]}] after which a goal cell is reachable, or null.
const DIRS = [[0, -1], [1, 0], [0, 1], [-1, 0]];  // the game's 0 north (-z), 1 east (+x), 2 south, 3 west
exports.DIR_KEYS = ['up+right', 'down+right', 'down+left', 'up+left'];
exports.planPushes = (grid, start, goals, blocks, maxStates = 20000) => {
  const W = grid.w, H = grid.h;
  const lvl0 = grid.cells.map((c) => c.level), slope = grid.cells.map((c) => c.slope), open = grid.cells.map((c) => c.open);
  const flag8 = grid.cells.map((c) => !!(c.flags & 8));
  const ix = (x, z) => z * W + x;
  const reach = (lvl, bset, from) => {
    const seen = new Set([from]), q = [from];
    while (q.length) {
      const c = q.pop(), x = c % W, z = (c / W) | 0;
      for (const [dx, dz] of DIRS) {
        const nx = x + dx, nz = z + dz;
        if (nx < 0 || nz < 0 || nx >= W || nz >= H) continue;
        const n = ix(nx, nz);
        if (seen.has(n) || !open[n] || bset.has(n)) continue;
        if (!(lvl[n] === lvl[c] || ((slope[n] || slope[c]) && Math.abs(lvl[n] - lvl[c]) <= 1))) continue;
        seen.add(n); q.push(n);
      }
    }
    return seen;
  };
  const goalIdx = [...goals].map((k) => { const [x, z] = k.split(',').map(Number); return ix(x, z); });
  const s0 = { lvl: lvl0.slice(), blocks: blocks.filter((b) => b.kind === 0).map((b) => ({ c: ix(b.x, b.z), pushes: b.pushes })), player: ix(...start), path: [] };
  const key = (st, region) => st.blocks.map((b) => b.c).sort((a, b) => a - b).join(',') + '|' + Math.min(...region) + '|' + st.lvl.join('');
  const q = [s0], seen = new Set();
  while (q.length && seen.size < maxStates) {
    const st = q.shift(), bset = new Set(st.blocks.map((b) => b.c)), region = reach(st.lvl, bset, st.player);
    if (goalIdx.some((c) => region.has(c))) return st.path;
    const k = key(st, region);
    if (seen.has(k)) continue;
    seen.add(k);
    st.blocks.forEach((b, bi) => {
      if (b.pushes <= 0) return;
      const bx = b.c % W, bz = (b.c / W) | 0, top = st.lvl[b.c];
      DIRS.forEach(([dx, dz], d) => {
        const px = bx - dx, pz = bz - dz, tx = bx + dx, tz = bz + dz;
        if (px < 0 || pz < 0 || tx < 0 || tz < 0 || px >= W || tx >= W || pz >= H || tz >= H) return;
        const P = ix(px, pz), T = ix(tx, tz);
        if (!region.has(P) || st.lvl[P] + 1 !== top) return;
        if (slope[T] || flag8[T] || bset.has(T) || !(st.lvl[T] < top)) return;
        const lvl = st.lvl.slice(), nb = st.blocks.map((x) => ({ ...x }));
        lvl[b.c] = top - 1;
        if (top - lvl[T] === 1) { lvl[T] = top; nb[bi].c = T; nb[bi].pushes--; }
        else { lvl[T] += 1; nb.splice(bi, 1); }  // fell in: now floor
        q.push({ lvl, blocks: nb, player: P, path: [...st.path, { x: bx, z: bz, dir: d, from: [px, pz] }] });
      });
    });
  }
  return null;
};

// Carry out one planned push: walk to the cell behind the block, hold toward it
// along the grid axis until the block leaves its cell (or give up).
exports.push = function* (step, opts = {}) {
  const r = yield* exports.navigate([step.from[0] * 256 + 128, step.from[1] * 256 + 128], { ...opts, noPush: true });
  if (!r.reached) return { done: false, reason: 'could not reach the push spot: ' + r.reason };
  const key = exports.DIR_KEYS[step.dir];
  const moved = () => !exports.blocks().some((b) => b.x === step.x && b.z === step.z && b.kind === 0);
  for (let t = 0; t < 180 && !moved(); t += 4) yield g.press(key, 4);
  yield g.until(() => exports.blocks().every((b) => b.kind === 0), 120);
  return { done: moved(), reason: moved() ? 'pushed' : 'did not move' };
};

// Enemies: the scene-actor registry list (gSceneActorRegistryNode: {next, actor},
// ending at a node with a NULL actor). Living as SceneActorRegistry_CountLiving
// counts them: health > 0 and status bit 0x20000 clear. src/scene/actor/registry/sub_080703A8.c.
exports.enemies = () => {
  const out = [];
  let node = g.u32(g.types.globals.gSceneActorRegistryNode.addr);
  for (let n = 0; node && n < 64; n++) {
    const actor = g.u32(node + 4);
    if (!actor) break;
    const a = g.at(actor, 'struct RoomActor');
    const pos = a.body.position;
    out.push({ addr: actor, category: a.category, health: a.health, alive: a.health > 0 && !(a.statusFlags & 0x20000),
      x: pos.x, y: pos.y, z: pos.z, state: a.state2A8, view: a });
    node = g.u32(node);
  }
  return out;
};

// Fight the room's living enemies (exports.enemies). Each step tries a few short
// tactics from a snapshot (move 12 frames in each of the 8 directions, attack with
// B, step toward the target and attack), rolls each forward `look` frames, and keeps
// the best by damage dealt minus lost life (life costs more: opts.lifeCost); the
// distance to the target breaks ties. B is the weapon (measured: it kills the first
// skeletons; A and the shoulder buttons do nothing in a fight).
// Targets the game itself tracks for lock-on (src/player/target_registry/sub_0815F5F8.c TargetRegistry:
// gSceneLinkList->first, entries {body, flags, callback, next}, flag 1 enabled): every
// hittable enemy, including the ones outside the scene actor registry (the Bronze Armor
// of 2803). body is a SceneActorBody (position at +12, owner at +60). Health by layout:
// flag 0x10 a RoomActor (health +0x9C); Armor_ReceiveTargetEvent the armor
// (src/monsters/armor/sub_0818B7AC.c ArmorContactOwner: health +0xB1C, max +0xB1E).
exports.targets = () => {
  const reg = g.u32(g.types.globals.gSceneLinkList.addr), out = [];
  if (!reg) return out;
  for (let e = g.u32(reg + 0x18), n = 0; e && n < 64; e = g.u32(e + 12), n++) {
    const body = g.u32(e), flags = g.u32(e + 4), cb = g.u32(e + 8);
    if (!body) continue;
    const owner = g.u32(body + 60), fn = cb ? g.name(cb) : '';
    let health = null;
    if (flags & 0x10) health = g.s16(owner + 0x9c);
    else if (/^Armor_ReceiveTargetEvent/.test(fn)) health = g.s16(owner + 0xb1c);
    out.push({ addr: owner, entry: e, enabled: !!(flags & 1), callback: fn, x: g.s16(body + 12), y: g.s16(body + 14), z: g.s16(body + 16),
      health, alive: health === null ? !!(flags & 1) : health > 0 });
  }
  return out;
};

exports.fight = function* (opts = {}) {
  const S = exports.session, p = $.gPlayers[0], start = g.frameNo, scene = exports.where().scene;
  const DIRS8 = ['up', 'down', 'left', 'right', 'up+left', 'up+right', 'down+left', 'down+right'];
  // a target that takes no damage for 40 steps is not a fight (a scripted thief,
  // an invulnerable actor): leave it alone
  const ignored = exports.fightIgnored || (exports.fightIgnored = new Set()), progress = new Map();
  // registry enemies, plus lock-on targets outside the registry (bosses, armors)
  const living = () => {
    const reg = exports.enemies().filter((e) => e.alive && !ignored.has(e.addr));
    const more = exports.targets().filter((t) => t.alive && t.health !== null && !reg.some((e) => e.addr === t.addr) && !ignored.has(t.addr));
    return reg.concat(more);
  };
  const hpSum = () => living().reduce((s, e) => s + Math.max(0, e.health), 0);
  const toward = (e) => {
    const dx = e.x - p.position.x, dz = e.z - p.position.z;
    const sx = Math.abs(dx) > 40 ? Math.sign(dx) : 0, sz = Math.abs(dz) > 40 ? Math.sign(dz) : 0;
    return KEY8[`${sx},${sz}`];
  };
  const life = () => exports.where().life ?? 0;
  let steps = 0;
  while (living().length && g.frameNo - start < (opts.frames ?? 6000) && exports.where().scene === scene) {
    if (yield* exports.clearInterruptions(opts)) continue;
    const target = living().sort((a, b) => Math.hypot(a.x - p.position.x, a.z - p.position.z) - Math.hypot(b.x - p.position.x, b.z - p.position.z))[0];
    const tk = toward(target);
    const tactics = [...DIRS8.map((k) => [[k, 12]]), [['b', 2], [0, 10]], ...(tk ? [[[tk, 6], ['b', 2], [0, 4]]] : [])];
    const root = S.snapshot(), hp0 = hpSum(), life0 = life();
    let best = null;
    for (const t of tactics) {
      S.restore(root);
      for (const [k, n] of t) yield g.press(k, n);
      if (exports.where().scene !== scene) continue;  // walked out through a door
      const mid = S.snapshot();
      yield g.press(0, opts.look ?? 20);
      if (exports.where().scene !== scene) continue;
      const dealt = hp0 - hpSum(), lost = life0 - life();
      const e = living()[0], dist = e ? Math.hypot(e.x - p.position.x, e.z - p.position.z) : 0;
      const score = dealt - (opts.lifeCost ?? 3) * lost - dist / 400;
      if (!best || score > best.score) best = { score, t, mid };
    }
    if (!best) { S.restore(root); break; }
    S.restore(best.mid);
    steps++;
    const pr = progress.get(target.addr) || { hp: target.health, since: steps };
    if (target.health < pr.hp) { pr.hp = target.health; pr.since = steps; }
    progress.set(target.addr, pr);
    if (steps - pr.since > 40) { ignored.add(target.addr); if (opts.trace) opts.trace({ ignore: target.addr, category: target.category }); }
    if (opts.trace && steps % 10 === 0) opts.trace({ fight: steps, hp: hpSum(), life: life(), enemies: living().length });
  }
  return { won: !living().length, life: life(), frames: g.frameNo - start, steps };
};
