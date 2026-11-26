// GameOver_OnUpdate (081636A0, game_over.c) dispatches by closeState.
// The manager exists throughout ordinary play, with closeState == 0.
// GameOver_UpdateCostSelection accepts A on choice 0 and runs the
// scene's continue procedure. B or choice 1 leads to a soft reset.
exports.gameOverMenu = {
  struct: 'struct GameOver',
  active: (panel) => panel.closeState !== 0,
  ready: (panel) => panel.closeState === 3,
  decide: (panel) => panel.choice === 0 ? 'a' : 'left',
};
exports.gameOverActive = () => !!$.gGameOver?.closeState;
