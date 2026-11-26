// FloorSwitch_AllocateSlot tracks eight 0x90-byte slots with the mask at
// manager + 0x498. FloorSwitch_UpdateOccupancy tests a 256-by-256 square
// from each slot's origin and FloorSwitch_RunStateScript calls r[0] on press.
exports.floorSwitches = () => {
  const symbol = g.types.globals.gFloorSwitchManager;
  const manager = symbol ? g.u32(symbol.addr) : 0;
  if (!manager) return [];
  const mask = g.u32(manager + 0x498), switches = [];
  const region = $.gGameState.region;
  for (let slot = 0; slot < 8; slot++) {
    if (!(mask & (1 << slot))) continue;
    const addr = manager + 0x18 + slot * 0x90;
    if (g.u32(addr + 0x54) !== region || !g.u32(addr + 0x4c)) continue;
    switches.push({ addr, name: g.u32(addr + 0x48), x: g.s16(addr + 0x1c) + 128,
      y: g.s16(addr + 0x1e), z: g.s16(addr + 0x20) + 128,
      release: Boolean(g.u32(addr + 0x50)), pressed: Boolean(g.u16(addr + 0x88) & 2) });
  }
  return switches;
};

const offsets = [[0, -1], [1, 0], [0, 1], [-1, 0]];
const delayedScripts = () => exports.tasks().filter((t) => !t.dead && t.update === 'DelayedScript_Update');

// FallingObjectManager_AddObject puts a new object at the head. Only an
// object with an attack collider landing on this plate makes it unsafe.
function fallingAttack(plate) {
  const head = $.gFallingObjectManager?.head;
  if (!head?.attack) return 0;
  const { x, z } = head.position;
  return Math.abs(x - plate.x) <= 128 && Math.abs(z - plate.z) <= 128 ? Number(head.$addr) : 0;
}

// A delayed press script can open the gate after the release script has run.
// Reverse at the first occupied tick, before the delayed task takes control.
exports.probeFloorSwitch = function* (plate, opts) {
  if (!plate.addr) return;
  const { x, y, z, scene } = exports.where();
  const grid = exports.grid();
  if (!grid || y !== plate.y) return;
  const [cx, cz] = exports.cellOf(plate.x, plate.z), cell = grid.at(cx, cz);
  if (!cell?.open) return;
  const start = exports.cellOf(x, z), avoidPlate = new Set([`${cx},${cz}`]);
  const approaches = offsets.map(([dx, dz], dir) => {
    const nearby = grid.at(cx + dx, cz + dz);
    if (!nearby?.open || nearby.level !== cell.level) return;
    const path = exports.findPath(grid, start, new Set([`${nearby.x},${nearby.z}`]), avoidPlate);
    if (path) return { target: [plate.x + dx * 256, plate.z + dz * 256], dir, length: path.length };
  }).filter(Boolean).sort((a, b) => a.length - b.length);
  if (!approaches.length) return;
  const session = exports.session, root = session.snapshot();
  const pressed = () => exports.floorSwitches().some((sw) => sw.addr === plate.addr && sw.pressed);
  for (const { target, dir } of approaches) {
    session.restore(root);
    const r = yield* exports.navigate(target, { ...opts, noPush: true, noBreak: true, stopIf: pressed });
    if (!r.reached || pressed() || exports.where().scene !== scene) continue;
    const existing = new Set(delayedScripts().map((t) => t.task));
    const fallBefore = fallingAttack(plate);
    const toward = exports.DIR_KEYS[(dir + 2) % offsets.length];
    for (let i = 0; i < 32 && !pressed(); i++) yield g.press(toward, 1);
    if (!pressed()) continue;
    const fall = fallingAttack(plate);
    if (fall && fall !== fallBefore) {
      session.restore(root);
      yield g.press(0, 1);
      return { hazard: true };
    }
    const pending = delayedScripts().find((t) => !existing.has(t.task));
    if (!plate.release || !pending) return { pressed: true };
    for (let i = 0; i < 8 && pressed(); i++) yield g.press(exports.DIR_KEYS[dir], 1);
    if (!pressed()) return { ready: () => !delayedScripts().some((t) => t.task === pending.task) };
  }
  session.restore(root);
  yield g.press(0, 1);
};
