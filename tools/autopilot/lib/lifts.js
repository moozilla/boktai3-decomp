exports.liftAt = (box) => {
  if (!box) return null;
  const global = g.types.globals.gNamedActorManager;
  const address = global ? g.u32(global.addr) : 0;
  if (!address) return null;
  const manager = g.at(address, 'struct NamedObjectManager');
  for (let i = 0; i < 12; i++) {
    if (!(manager.used & (1 << i))) continue;
    const lift = manager.slots[i], { x, z } = lift.body.position;
    if (lift.kind === 2 && lift.lowerHeight < lift.upperHeight && x >= box.x0 && x < box.x1 && z >= box.z0 && z < box.z1)
      return lift;
  }
  return null;
};

// NamedObject_UpdateTerrainRegistration exposes the lift's floor tile only
// once the player reaches its height. Route to a landing first, then the
// normal door skill can board and wait for the lift's contact timer.
exports.approachLift = function* (door, opts = {}) {
  const box = exports.boxes().find((b) => b.name === door);
  const lift = exports.liftAt(box), player = $.gPlayers[0];
  if (!lift || !player) return null;
  if (lift.state === 2) yield g.until(() => lift.state !== 2, 180);
  const { x, y, z } = lift.body.position;
  if (player.position.y === y) return null;
  const [cx, cz] = exports.cellOf(x, z), grid = exports.grid();
  if (!grid) return null;
  const goals = new Set();
  for (const [dx, dz] of [[0, 1], [1, 0], [0, -1], [-1, 0]]) {
    const cell = grid.at(cx + dx, cz + dz);
    if (cell?.open && cell.level === (y >> 8)) goals.add(`${cell.x},${cell.z}`);
  }
  if (!goals.size) return null;
  const here = exports.cellOf(player.position.x, player.position.z);
  const path = exports.findPath(grid, here, goals);
  if (!path) return null;
  const target = path.at(-1).map((coord) => coord * 256 + 128);
  const travel = exports.coffinAvailable() ? exports.carryCoffin : exports.navigate;
  const result = yield* travel(target, { ...opts, tolerance: 24 });
  yield g.press(0, 1);
  return { name: lift.name, height: y, target, ...result };
};
