// Skeleton_UpdateDefeat leaves one HP when it installs the hidden timeout.
// A revival delay above 19999 runs the defeat script and disables revival.
exports.skeletonDefeated = (actor) => {
  if (actor.state2A8 !== 3 || actor.controlKind !== 0x39 ||
      g.name(actor.update) !== 'Skeleton_UpdateBehaviorCallbacks') return false;
  const skeleton = g.at(actor.$addr, 'struct Skeleton');
  return skeleton.revivalDelay > 19999 && Boolean(skeleton.actionFlags & 0x10);
};
