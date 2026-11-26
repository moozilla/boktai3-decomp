const DIRECTION = { '0,-1': 'up+right', '1,0': 'down+right', '0,1': 'down+left', '-1,0': 'up+left',
  '1,1': 'down', '-1,-1': 'up', '1,-1': 'right', '-1,1': 'left' };
const direction = (dx, dz) => {
  const major = Math.max(Math.abs(dx), Math.abs(dz)) || 1;
  return DIRECTION[`${Math.round(dx / major)},${Math.round(dz / major)}`];
};

exports.purificationBattle = () => $.gScenePanel?.initialMode === 1;

// ScenePanelRig_TickResourceDrain drains darkness four times as fast with
// the rig over the coffin (state 2). When it wanders or approaches a
// generator, try gunfire from the outside of its route to push it back.
exports.fightPurification = function* (opts = {}) {
  const panel = $.gScenePanel, rig = panel.rig, S = exports.session;
  const start = g.frameNo, story = exports.story(), scene = exports.where().scene;
  const changed = () => exports.where().scene !== scene || exports.story() > story;
  const darkness = () => $.gGameState.unk618;
  const life = () => $.gPlayers[0]?.currentStatA ?? 0;
  const trace = opts.trace || (() => {});
  let steps = 0, equipped = false;
  S.setSolar(64);
  while (!changed() && life() > 0 && g.frameNo - start < (opts.frames ?? 5000)) {
    if (yield* exports.clearInterruptions({ ...opts })) continue;
    if (g.fn.ScriptUtility_IsControlFlagSet() || panel.effectState !== 10 || darkness() === 0 ||
        rig.state < 3 || rig.state > 5 || rig.colliderHold) {
      yield g.press(0, 30);
      continue;
    }
    if (!equipped) {
      const result = yield* exports.quickChange({ gun: true });
      trace({ weapon: result });
      equipped = true;
      continue;
    }
    const player = $.gPlayers[0], p = player.position;
    const { x, z } = rig.position, centerX = rig.origin.x, centerZ = rig.origin.z;
    const outside = direction(x + (x - centerX) / 2 - p.x, z + (z - centerZ) / 2 - p.z);
    const aim = direction(x - p.x, z - p.z);
    const actions = [[[0, 24]], ...exports.gunTactics(aim)];
    if (outside) actions.push([[outside, 16], [0, 8]]);
    const root = S.snapshot(), darkness0 = darkness(), life0 = life();
    let best = null;
    for (const action of actions) {
      S.restore(root);
      for (const [key, frames] of action) yield g.press(key, frames);
      if (changed()) return { done: true };
      const dist = Math.hypot(rig.position.x - centerX, rig.position.z - centerZ);
      const score = (darkness0 - darkness()) * 3 + panel.activeCount * 40 + (rig.state === 2 ? 30 : 0)
        - dist / 12 - (life0 - life()) * 8 - (life() > 0 ? 0 : 10000);
      if (!best || score > best.score) best = { score, snapshot: S.snapshot() };
    }
    S.restore(best.snapshot);
    yield g.press(0, 1);
    if (++steps % 10 === 0) trace({ steps, darkness: darkness(), life: life(), state: rig.state, active: panel.activeCount });
  }
  return { done: changed(), steps, darkness: darkness(), life: life() };
};
