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

exports.equipBestFrame = function* () {
  const active = exports.weapon(), owned = exports.bags().frames;
  if (owned.length < 2 || !active.gun) return null;
  const [best] = exports.rankGunFrames(owned, active.lens);
  if (!best || best.id === active.frame) return null;
  const result = yield* exports.menuSearch(() => exports.weapon().frame === best.id, { page: 3, max: 120 });
  return { kind: 'frame', id: best.id, power: best.power, cost: best.cost, ...result };
};
