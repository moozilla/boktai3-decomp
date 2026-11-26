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
  const session = exports.session, root = session.snapshot(), options = { page: 3, max: 120 };
  let result = { done: true };
  if (assigned.length === 8) {
    const worst = exports.rankGunFrames(assigned, active.lens).at(-1);
    result = yield* exports.menuSearch(() => !exports.gunLoadout().includes(worst.id), options);
  }
  if (result.done) result = yield* exports.menuSearch(() => exports.weapon().frame === best.id, options);
  if (!result.done) {
    session.restore(root);
    yield g.press(0, 1);
  }
  return { ...details, ...result, method: 'pause menu' };
};
