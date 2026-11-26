// The cartridge threshold counts sensor clocks, so smaller values mean more light.
// Solar_OnTimerInterrupt (08243D28) needs a low input to enter its measurement
// phase. Use a positive threshold, even in bright sunlight.
const BRIGHT_LUX = 64;

exports.solarStatus = () => ({
  measurement: $.gSolarMeasurement | 0,
  intensity: $.gGameState?.unk874 ?? 0,
  level: $.gGameState?.unk876 ?? 0,
});

// AreaEffect_AccumulatePlayerResource (0804E618) spends 120 units per energy
// point. Player.statusFlags 0x10 permits charging and 0x20 charges eight times
// as fast (0804E730). More sunlight by itself does not recharge the gun indoors.
exports.chargeEnergy = function* (need, opts = {}) {
  const session = exports.session, before = exports.weapon().energy;
  if (before >= need) return { done: true, energy: before };
  const player = $.gPlayers[0];
  if (!player || !(player.statusFlags & 0x30)) return { done: false, energy: before, reason: 'player is in shade' };
  const previous = session.solar;
  session.setSolar(opts.lux ?? BRIGHT_LUX);
  const start = g.frameNo;
  if (opts.trace) opts.trace({ threshold: session.solar, before });
  try {
    while (g.frameNo - start < (opts.frames ?? 900) && exports.weapon().energy < need) {
      if (!$.gPlayers[0] || !(player.statusFlags & 0x30)) break;
      if (opts.stopIf?.()) break;
      yield g.press(0, 15);
    }
    const energy = exports.weapon().energy;
    return { done: energy >= need, energy, before, frames: g.frameNo - start, ...exports.solarStatus() };
  } finally {
    session.setSolar(opts.restore ?? previous);
  }
};
