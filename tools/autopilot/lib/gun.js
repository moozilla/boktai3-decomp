// Player_GetSpecialAttackDescriptor (0814DCD0) indexes a four-byte record by
// Player.statModifierContext.actionKind. Player_SpawnSelectedAttackProjectile
// (0814DFDC) uses byte 2 as its energy cost, except lens mode 1 is free.
const FRAME_TABLE = 0x08e60f4c;
function frameDescriptor(action, lens) {
  if (action <= 0) return null;
  const descriptor = FRAME_TABLE + (action - 1) * 4;
  return {
    action,
    power: g.u8(descriptor + 1),
    cost: lens === 1 ? 0 : g.u8(descriptor + 2),
  };
}

exports.gunFrame = () => {
  const player = $.gPlayers[0], weapon = exports.weapon();
  return weapon.gun && player ? frameDescriptor(player.statModifierContext.actionKind, weapon.lens) : null;
};

// Player_UpdateSelectedAttackCharge (0814E3D8) allows aiming while B is held
// and releases the shot after the windup. Action 2 instead fires a burst.
exports.gunTactics = (direction) => {
  const frame = exports.gunFrame();
  if (!frame || exports.weapon().energy < frame.cost) return [];
  const aim = direction ? [[direction, 4]] : [];
  const fire = direction ? direction + '+b' : 'b';
  return [
    [...aim, [fire, 2], [0, 18]],
    [...aim, [fire, frame.action === 2 ? 14 : 26], [0, 15]],
  ];
};

// GameState_GetUnlockEntry (0822E618) maps the owned frame record to its
// action. Choose power per energy, breaking ties with the stronger shot.
exports.rankGunFrames = (owned, lens) => owned.map((id) => {
  const descriptor = frameDescriptor(g.u8(FRAME_TABLE + id * 4), lens);
  return descriptor ? { id, ...descriptor } : null;
}).filter(Boolean).sort((a, b) =>
  b.power / Math.max(1, b.cost) - a.power / Math.max(1, a.cost) || b.power - a.power);

// GameState_ToggleUnlockEquipSlot stores up to eight inventory indexes at
// 0x78. Quick Change cycles these without reopening the inventory grid.
exports.gunLoadout = () => {
  const state = g.u32(g.types.globals.gGameState.addr), frames = [];
  for (let i = 0; i < 8; i++) {
    const slot = g.s16(state + 0x78 + i * 2);
    if (slot < 0 || slot >= 16) continue;
    const id = g.u16(state + 0x4c0 + slot * 2) - 1;
    if (id >= 0) frames.push(id);
  }
  return frames;
};

// Hud_MoveGridUp/Down/Left/Right wraps each axis and skips empty cells.
exports.gunGridRoute = (cells, x, y, target, verticalOnly = false) => {
  const steps = [['up', 0, -1], ['down', 0, 1], ['left', -1, 0], ['right', 1, 0]];
  const queue = [[x, y, []]], seen = new Set([x * 4 + y]);
  for (const [x, y, path] of queue) {
    const current = cells[x * 4 + y];
    if (current === target) return path;
    for (const [key, dx, dy] of verticalOnly ? steps.slice(0, 2) : steps) {
      for (let n = 1; n < (dx ? 9 : 4); n++) {
        const nx = (x + dx * n + 9) % 9, ny = (y + dy * n + 4) % 4;
        const index = nx * 4 + ny, value = cells[index];
        if (value === 0xff || value === current) continue;
        if (!seen.has(index)) {
          seen.add(index);
          queue.push([nx, ny, [...path, key]]);
        }
        break;
      }
    }
  }
  return null;
};

function* changeGunGrid(changes) {
  const field = exports.menuState()?.callback;
  if (!field) return false;
  const secondary = 'SceneHud_UpdateSecondaryActionSelection';
  const action = 'SceneHud_UpdateActionGridInput';
  const pulse = function* (key, delay = 14) { yield g.press(key, 2); yield g.press(0, delay); };
  const select = function* (target, verticalOnly) {
    // SceneInputGridView at SceneHudState.actionInput, followed by 36 cells.
    const grid = exports.hudTask() + 0x4384;
    const path = exports.gunGridRoute(g.bytes(grid + 12, 36), g.u8(grid + 1), g.u8(grid + 2), target, verticalOnly);
    if (!path) return false;
    for (const key of path) yield* pulse(key);
    if (g.u8(grid + 3) !== target) return false;
    yield* pulse('a');
    return true;
  };
  yield* pulse('start', 60);
  for (let i = 0; i < 8 && exports.menuState()?.sub !== secondary; i++) yield* pulse('r', 40);
  if (exports.menuState()?.sub !== secondary || !(yield* select(32, true))) return false;
  if (exports.menuState()?.sub !== action) return false;
  for (const { cell, changed } of changes) {
    if (!(yield* select(cell, false)) || !changed()) return false;
  }
  for (let i = 0; i < 3 && exports.menuState()?.callback !== field; i++) yield* pulse('b', 35);
  return exports.menuState()?.callback === field;
}

function inventorySlot(state, offset, length, value) {
  for (let i = 0; i < length; i++) if (g.u16(state + offset + i * 2) === value) return i;
  return -1;
}

function* setGunLoadout(best, worst) {
  const state = g.u32(g.types.globals.gGameState.addr);
  const slotOf = (id) => inventorySlot(state, 0x4c0, 12, id + 1);
  const bestSlot = slotOf(best), worstSlot = worst === undefined ? null : slotOf(worst);
  if (bestSlot < 0 || worstSlot === -1) return false;
  // The frame grid's cells 8–19 map to the twelve unlock inventory slots.
  const changes = [];
  if (worstSlot !== null) changes.push({ cell: 8 + worstSlot, changed: () => !exports.gunLoadout().includes(worst) });
  changes.push({ cell: 8 + bestSlot, changed: () => exports.weapon().frame === best });
  return yield* changeGunGrid(changes);
}

function* setGunLens(id) {
  const state = g.u32(g.types.globals.gGameState.addr);
  const cell = inventorySlot(state, 0x828, 8, id);
  if (cell < 0) return false;
  // SolarFruit_ToggleEquipSlot selects a lens when it inserts it. An
  // assigned lens needs to be removed and inserted again to select it.
  const assigned = () => inventorySlot(state, 0x90, 8, cell) >= 0;
  const changes = [];
  if (assigned()) changes.push({ cell, changed: () => !assigned() });
  changes.push({ cell, changed: () => exports.weapon().lens === id });
  return yield* changeGunGrid(changes);
}

exports.equipBestFrame = function* () {
  const active = exports.weapon(), owned = exports.bags().frames;
  if (owned.length < 2 || !active.gun) return null;
  const [best] = exports.rankGunFrames(owned, active.lens);
  if (!best || best.id === active.frame) return null;
  const details = { kind: 'frame', id: best.id, power: best.power, cost: best.cost };
  const assigned = exports.gunLoadout();
  if (assigned.includes(best.id)) {
    const result = yield* exports.quickChange({ gun: true, frame: best.id });
    return { ...details, ...result, method: 'quick change' };
  }
  const session = exports.session, root = session.snapshot(), start = g.frameNo;
  const worst = assigned.length === 8 ? exports.rankGunFrames(assigned, active.lens).at(-1)?.id : undefined;
  const done = yield* setGunLoadout(best.id, worst);
  if (!done) {
    session.restore(root);
    yield g.press(0, 1);
  }
  return { ...details, done, frames: g.frameNo - start, method: 'pause menu' };
};

exports.equipEnergyFreeGun = function* () {
  // Player_SpawnSelectedAttackProjectile waives the frame cost for Luna.
  if (!exports.bags().lenses.includes(1)) return null;
  const session = exports.session, root = session.snapshot();
  const lens = yield* exports.quickChange({ gun: true, lens: 1 });
  let done = lens.done;
  if (!done) {
    session.restore(root);
    const gun = yield* exports.quickChange({ gun: true });
    done = gun.done && (yield* setGunLens(1));
  }
  if (done) {
    const rankedFrame = yield* exports.equipBestFrame();
    return { done: true, ...exports.weapon(), rankedFrame };
  }
  session.restore(root);
  yield g.press(0, 1);
  return { done: false, ...exports.weapon() };
};
