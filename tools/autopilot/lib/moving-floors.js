const CARDINAL = [[0, 1], [1, 0], [0, -1], [-1, 0]];

function floorManager() {
  const definition = g.types.globals.gNamedActorManager;
  const address = definition ? g.u32(definition.addr) : 0;
  return address ? g.at(address, 'struct NamedObjectManager') : null;
}

exports.movingFloors = () => {
  const manager = floorManager(), floors = [];
  if (manager) for (let i = 0; i < 12; i++) {
    if (manager.used & (1 << i) && manager.slots[i].kind === 1) floors.push(manager.slots[i]);
  }
  return floors;
};

// NamedObject_RegisterFollowCells temporarily gives these cells a floor.
// Check the authored cells so a moving platform does not look like a bridge.
function floorGaps(grid) {
  const cells = $.gTerrainMap.cells;
  const base = Number(cells.$addr ?? cells);
  const bytes = g.bytes(base + 12, grid.w * grid.h * 4);
  return new Set(grid.cells.filter((cell) => bytes[cell.index * 4 + 2] & 0xc0).map((cell) => `${cell.x},${cell.z}`));
}

function routePoints(floor, header) {
  const route = header + 4 + floor.routeId * 4;
  const count = g.u8(route), offset = g.u16(route + 2);
  return Array.from({ length: count }, (_, i) => {
    const point = header + offset + i * 8;
    return { x: g.s16(point), z: g.s16(point + 2), wait: g.u8(point + 4) };
  });
}

exports.planMovingFloor = (grid, here, goals, gaps, floors) => {
  const sides = (point, y) => {
    const [x, z] = exports.cellOf(point.x, point.z);
    return CARDINAL.map(([dx, dz]) => grid.at(x + dx, z + dz))
      .filter((cell) => cell?.open && cell.level === (y >> 8) && !gaps.has(`${cell.x},${cell.z}`));
  };
  let best;
  for (const { floor, points, y } of floors) for (let i = 0; points.length > 1 && i < points.length; i++) {
    const board = points[i], arrival = points[(i + 1) % points.length];
    for (const launch of sides(board, y)) {
      const approach = exports.findPath(grid, here, new Set([`${launch.x},${launch.z}`]), gaps);
      if (!approach) continue;
      for (const leave of sides(arrival, y)) {
        const finish = exports.findPath(grid, [leave.x, leave.z], goals, gaps);
        const cost = finish && approach.length + finish.length;
        if (cost && (!best || cost < best.cost)) best = { floor, launch, board, arrival, leave, cost };
      }
    }
  }
  return best;
};

exports.approachMovingFloor = function* (boxName, opts = {}) {
  const floors = exports.movingFloors(), routes = $.gTerrainMap?.routes;
  const box = floors.length && exports.boxes().find((entry) => entry.name === boxName);
  if (!routes || !box) return null;
  const grid = exports.grid(), player = $.gPlayers[0];
  if (!player || !grid) return null;
  const gaps = floorGaps(grid), here = exports.cellOf(player.position.x, player.position.z);
  const blocked = new Set(gaps);
  for (const entry of exports.boxes().filter((entry) => opts.avoid?.includes(entry.name))) {
    for (let z = entry.z0 >> 8; z <= (entry.z1 - 1) >> 8; z++) {
      for (let x = entry.x0 >> 8; x <= (entry.x1 - 1) >> 8; x++) blocked.add(`${x},${z}`);
    }
  }
  const goals = new Set(grid.cells.filter((cell) => cell.open && !gaps.has(`${cell.x},${cell.z}`) &&
    cell.x >= box.x0 >> 8 && cell.x <= (box.x1 - 1) >> 8 &&
    cell.z >= box.z0 >> 8 && cell.z <= (box.z1 - 1) >> 8).map((cell) => `${cell.x},${cell.z}`));
  if (!goals.size || exports.findPath(grid, here, goals, blocked)) return { gaps };
  const header = Number(routes.$addr ?? routes);
  const plan = exports.planMovingFloor(grid, here, goals, blocked,
    floors.map((floor) => ({ floor, points: routePoints(floor, header), y: floor.body.position.y })));
  if (!plan) return { gaps };
  const { floor, launch, board, arrival, leave } = plan;
  const session = exports.session, root = session.snapshot(), scene = exports.where().scene;
  const docked = (point) => floor.state === 3 && (floor.flags & 1) &&
    floor.body.position.x === point.x && floor.body.position.z === point.z;
  const attached = () => floorManager()?.followName === floor.name;
  const sameRoom = () => exports.where().scene === scene;
  const cross = function* () {
    const approach = yield* exports.navigate([launch.x * 256 + 128, launch.z * 256 + 128], {
      avoid: opts.avoid, avoidCells: gaps, tolerance: 24,
    });
    if (!approach.reached || !sameRoom()) return 'launch landing unreachable';
    const ready = () => docked(board) && floor.route.unknown3 + 40 < board.wait;
    yield g.until(ready, 850);
    if (!ready()) return 'floor did not dock';
    yield* exports.stepTo(board.x >> 8, board.z >> 8, { tolerance: 24, frames: 45 });
    if (!attached()) return 'boarding did not attach';
    yield g.until(() => docked(arrival) || !attached() || !sameRoom(), 850);
    if (!sameRoom() || !attached() || !docked(arrival)) return 'floor did not carry to the landing';
    return (yield* exports.stepTo(leave.x, leave.z, { frames: 45 })).reached ? null : 'departure landing unreachable';
  };
  const reason = yield* cross(), done = reason === null;
  if (!done) {
    session.restore(root);
    yield g.press(0, 1);
  }
  return { gaps, done, reason, name: floor.name, board, arrival, leave: [leave.x, leave.z] };
};
