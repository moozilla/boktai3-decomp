// Both form cues require a full statHundredths gauge outside special modes.
exports.darkTranceReady = () => {
  const player = $.gPlayers[0];
  return Boolean(player && player.actionMode === 0 && player.tetherMode <= 1 &&
    player.statHundredths >= 1000 && !player.actionRequestBlocked && !player.actionRequestRestricted);
};
exports.solarTranceReady = exports.darkTranceReady;

function* enterTrance(ability, mode) {
  const session = exports.session, root = session.snapshot();
  const player = $.gPlayers[0], selected = () => g.fn.Player_GetSelectedAbility(player.$addr);
  for (let i = 0; i < 8 && selected() !== ability; i++) {
    yield g.press('select', 6);
    yield g.press('select+l', 4);
    yield g.press(0, 20);
  }
  if (selected() === ability) {
    yield g.press('l', 2);
    yield g.until(() => player.actionMode === mode, 180);
    if (player.actionMode === mode) return true;
  }
  session.restore(root);
  return false;
}

// Player_SelectRequestedActionMode maps abilities 2 and 1 to motions 10 and
// 11. Their cues enter character action modes 1 and 2 respectively.
exports.enterDarkTrance = () => enterTrance(2, 1);
exports.enterSolarTrance = () => enterTrance(1, 2);

// Action mode 1 maps A to Dark Claw and B to Dark Fang. The dialogue teaches
// repeated B presses for a bite, so search a repeat as well as a single hit.
exports.darkTranceTactics = (direction) => {
  const aim = direction ? [[direction, 4]] : [];
  return [
    [...aim, ['a', 2], [0, 18]],
    [...aim, ['b', 2], [0, 6], ['b', 2], [0, 14]],
  ];
};

// In Solar Trance A and B request action 38. The effect uses two energy.
exports.solarTranceTactics = (direction) => {
  const aim = direction ? [[direction, 4]] : [];
  return [
    [['a', 2], [0, 20]],
    ...(aim.length ? [[...aim, ['a', 2], [0, 20]]] : []),
  ];
};

// SolarBomb_Init sets a 120-frame primed fuse after the 30-frame assembly.
// The five explosions start up to 16 frames apart and refresh for eight frames.
exports.solarBombLook = (elapsed) => Math.max(0, 180 - elapsed);
