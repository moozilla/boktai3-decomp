// The key door procedure stores its unlock bit in r708[0x62]. The script
// uses Inventory_HasItemFromScript and consumes a key only after opening.
exports.closedLock = (locks, box) => {
  const addr = g.types.globals.gGameVariables.addr;
  const bank = g.u32(addr);
  if (!bank) return undefined;
  return locks.find((lock) => lock.box === box && !(g.u8(bank + 0x62 + (lock.bit >> 3)) & (1 << (lock.bit & 7))));
};

// Proc 1185 assigns bit + 1 as the slot ID. SlidingDoors_ScriptAppend uses
// the registered controller pool, and SlidingDoor_InitSlotPlacement applies
// the facing-dependent offset before the interaction radius is checked.
exports.keyDoorPosition = (lock) => {
  const address = g.fn.ResourceRegistry_GetResource(0x85e6);
  if (!address) return undefined;
  const pool = g.at(address, 'struct SceneObjPool');
  if (!pool.slots) return undefined;
  for (let i = 0; i < pool.count; i++) {
    const slot = pool.slots.$at(i);
    if (slot.id !== lock.bit + 1) continue;
    const { x, y, z } = slot.pos.signedPosition;
    return { x, y, z, direction: slot.dir };
  }
};

// The controller accepts A within its interaction radius and three facings
// from SlidingDoor_CheckActorCompatibility. Approach on the usable side.
const offsets = [[128, 0], [0, 128], [-128, 0], [0, -128]];
const facings = [
  ['left', 'up+left', 'up'], ['up', 'up+right', 'right'],
  ['right', 'down+right', 'down'], ['down', 'down+left', 'left'],
];

exports.openKeyDoor = function* (lock, opts) {
  const door = exports.keyDoorPosition(lock);
  if (!door || !offsets[door.direction]) return { done: false, reason: 'no door object' };
  const { x, y, z, direction } = door;
  const close = () => {
    const state = $.gGameState;
    return (state.positionX - x) ** 2 + (state.positionZ - z) ** 2 <= 0x12cc7 && Math.abs(state.positionY - y) <= 0xfe;
  };
  if (!close()) {
    const [dx, dz] = offsets[direction];
    const approach = yield* exports.navigate([x + dx, z + dz], opts);
    if (!close()) return { done: false, reason: approach.reason, x, z };
  }
  const session = exports.session, root = session.snapshot();
  const opened = () => !exports.closedLock([lock], lock.box);
  for (const face of facings[direction]) {
    session.restore(root);
    yield g.press(face, 4);
    yield g.press('a', 2);
    yield g.until(opened, 60);
    if (opened()) return { done: true, x, z, face };
  }
  session.restore(root);
  return { done: false, reason: 'lock bit unchanged', x, z };
};
