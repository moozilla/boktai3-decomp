exports.bikeRider = () => {
  const task = exports.tasks().find((t) => !t.dead && t.update === 'CoffinBike_UpdateTask');
  return task ? g.at(Number(task.task), 'struct Glove') : null;
};

// CoffinBike_UpdatePostState applies damage to style.initialWord. The course moves
// along X and steering changes Z. First try ordinary acceleration. Search
// steering and bike actions when that trial takes damage or stops advancing.
exports.rideBike = function* (bike, opts = {}) {
  if (g.name(bike.update) !== 'CoffinBike_UpdateStopping') {
    yield g.press('r', 4);
    return null;
  }
  const session = exports.session, motion = bike.motion;
  const scene = exports.where().scene, story = exports.story(), horizon = opts.horizon ?? 100;
  const x0 = motion.position.x, life0 = motion.style.initialWord;
  const root = session.snapshot();
  const end = () => exports.where().scene !== scene || exports.story() > story || exports.message().open;
  const state = () => ({ x: motion.position.x >> 12, lane: motion.position.z >> 12, life: motion.style.initialWord });
  const drive = function* (key, frames) {
    for (let left = frames; left > 0 && !end(); left -= 8) yield g.press(key, Math.min(left, 8));
  };
  yield* drive('r', horizon);
  if (end() || (motion.style.initialWord >= life0 && motion.position.x > x0)) return null;

  const score = () => (motion.position.x - x0) / 4096 + (motion.style.initialWord - life0) * 12
    - (motion.style.initialWord > 0 ? 0 : 10000);
  let best = { key: 'r', frames: horizon, score: score(), snapshot: session.snapshot() };
  for (const [key, frames] of [['r+up', 24], ['r+down', 24], ['r+a', 2], ['r+b', 24], ['r+up', 56], ['r+down', 56]]) {
    session.restore(root);
    const actionFrames = Math.min(frames, horizon);
    yield* drive(key, actionFrames);
    yield* drive('r', horizon - actionFrames);
    if (end()) return { key, frames: actionFrames, ...state() };
    const result = score();
    if (result > best.score) best = { key, frames: actionFrames, score: result, snapshot: session.snapshot() };
  }
  session.restore(best.snapshot);
  yield g.press(0, 1);
  return { key: best.key, frames: best.frames, ...state() };
};
