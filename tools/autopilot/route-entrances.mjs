// Equal-length routes can arrive in different parts of the same room. Rank
// entrances by how close the script places them to the next forward exit.
export function distanceFromEntrance(door, stageOf, distances) {
  const stage = stageOf(door.scene);
  const entrance = stage.entrances.find((entry) => entry.id === door.entrance);
  const hops = distances.get(stage.scene);
  if (entrance?.x === undefined || entrance.z === undefined || !hops) return Infinity;
  let distance = Infinity;
  for (const exit of stage.doors) {
    if (!(distances.get(exit.scene) < hops) || exit.x === undefined || exit.z === undefined) continue;
    distance = Math.min(distance, Math.hypot(exit.x - entrance.x, exit.z - entrance.z));
  }
  return distance;
}

// Some story scenes run the advancing script only from a special entrance.
export function storyEntrances(stage, milestone, conditionsAllow) {
  const threshold = `progress<${milestone}`;
  const matches = stage.entrances.filter((entry) => entry.cond.some((cond) => cond.replace(/[()\s]/g, '') === threshold) &&
    conditionsAllow(entry.cond));
  return matches.length ? new Set(matches.map((entry) => entry.id)) : undefined;
}

// Traverse incoming doors once to measure walking hops to a goal. Ignore
// failed approaches and a locked door being bypassed on a key detour.
export function routeDistances(stages, goal, failed, locked, entrances) {
  if (goal < 0) return new Map();
  // An ordinary arrival can still be on the route to the scripted arrival.
  const terminal = entrances ? Symbol('scripted arrival') : goal;
  const incoming = new Map();
  for (const stage of stages) for (const door of stage.doors) {
    const edge = `${stage.scene}:${door.box}`;
    if (edge === locked || failed.has(edge)) continue;
    const to = door.scene === goal && entrances?.has(door.entrance) ? terminal : door.scene;
    if (!incoming.has(to)) incoming.set(to, []);
    incoming.get(to).push(stage.scene);
  }
  const distances = new Map([[terminal, 0]]), pending = [terminal];
  for (const room of pending) {
    const hops = distances.get(room) + 1;
    for (const from of incoming.get(room) ?? []) {
      if (distances.has(from)) continue;
      distances.set(from, hops);
      pending.push(from);
    }
  }
  if (entrances) distances.delete(terminal);
  return distances;
}
