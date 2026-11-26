// The shared escort setup (proc 845) installs proc 841 on the tether actor.
exports.coffinAvailable = () => $.gTetherActor?.interactionScriptId === 841;

// Player_TestNearbyInteraction checks the range and the two actors' terrain.
// Face the coffin and press A. Once player mode 6 is active, keep A held
// through walking and snapshot moves until a script takes over.
exports.carryCoffin = function* (door, opts) {
  const player = $.gPlayers[0], coffin = $.gTetherActor;
  const attached = () => player.tetherMode === 6;
  const nearby = () => g.fn.Player_TestNearbyInteraction(player.$addr);
  if (!attached()) {
    if (!nearby()) {
      yield* exports.navigate([coffin.position.x, coffin.position.z], { ...opts, stopIf: nearby });
      if (!nearby()) return { grabbed: false, reason: 'coffin out of reach' };
    }
    const session = exports.session, root = session.snapshot();
    for (const dir of [null, 0, 1, 2, 3]) {
      session.restore(root);
      if (dir !== null) {
        yield g.press(exports.DIR_KEYS[dir], 14);
        yield g.press(exports.DIR_KEYS[(dir + 2) % 4], 4);
      }
      if (!nearby()) continue;
      yield g.press('a', 2);
      if (attached()) break;
    }
    if (!attached()) {
      session.restore(root);
      yield g.press(0, 1);
      return { grabbed: false, reason: 'could not grab coffin' };
    }
  }
  const keys = g.keys;
  let reading = false;
  const lostGrip = () => !reading && !attached() && !g.fn.ScriptUtility_IsControlFlagSet();
  g.keys = keys | g.keyMask('a');
  try {
    const route = yield* exports.navigate(door, { ...opts,
      stopIf: () => lostGrip() || opts.stopIf?.(),
      beforeText: () => { reading = true; g.keys = keys; } });
    if (!route.reached && lostGrip()) return { grabbed: true, ...route, reason: 'lost grip' };
    return { grabbed: true, ...route };
  } finally {
    g.keys = keys;
  }
};
