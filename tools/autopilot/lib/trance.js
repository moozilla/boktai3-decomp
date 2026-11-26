// Player_SelectRequestedActionMode maps ability 2 to motion 10. Its form
// cue enters actionMode 1 if the statHundredths gauge has reached 1000.
exports.darkTranceReady = () => {
  const player = $.gPlayers[0];
  return Boolean(player && player.actionMode === 0 && player.tetherMode <= 1 &&
    player.statHundredths >= 1000 && !player.actionRequestBlocked && !player.actionRequestRestricted);
};

exports.enterDarkTrance = function* () {
  const session = exports.session, root = session.snapshot();
  const player = $.gPlayers[0], selected = () => g.fn.Player_GetSelectedAbility(player.$addr);
  for (let i = 0; i < 8 && selected() !== 2; i++) {
    yield g.press('select', 6);
    yield g.press('select+l', 4);
    yield g.press(0, 20);
  }
  if (selected() === 2) {
    yield g.press('l', 2);
    yield g.until(() => player.actionMode === 1, 180);
    if (player.actionMode === 1) return true;
  }
  session.restore(root);
  return false;
};

// Action mode 1 maps A to Dark Claw and B to Dark Fang. The dialogue teaches
// repeated B presses for a bite, so search a repeat as well as a single hit.
exports.darkTranceTactics = (direction) => {
  const aim = direction ? [[direction, 4]] : [];
  return [
    [...aim, ['a', 2], [0, 18]],
    [...aim, ['b', 2], [0, 6], ['b', 2], [0, 14]],
  ];
};
