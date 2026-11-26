exports.solvePlatePuzzle = function* (puzzle, opts) {
  const goalPlates = new Set(puzzle.plates);
  let moves = 0;
  while (moves < 12 && exports.conditionsAllow(puzzle.cond)) {
    const plates = exports.floorSwitches().filter((plate) => goalPlates.has(plate.name));
    if (plates.length !== goalPlates.size) break;
    const grid = exports.grid(), { x, z } = exports.where();
    if (!grid) break;
    const goals = new Set(plates.map((plate) => exports.cellOf(plate.x, plate.z).join(',')));
    const plan = exports.planPushes(grid, exports.cellOf(x, z), goals, exports.blocks(), 3500, { occupy: true });
    if (!plan?.length) break;
    const result = yield* exports.push(plan[0], opts);
    if (opts.trace) opts.trace({ push: plan[0], ...result });
    if (!result.done) break;
    moves++;
  }
  return { done: !exports.conditionsAllow(puzzle.cond), moves };
};
