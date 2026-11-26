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

// Traverse incoming doors once to measure walking hops to a goal. Ignore
// failed approaches and a locked door being bypassed on a key detour.
export function routeDistances(stages, goal, failed, locked) {
  if (goal < 0) return new Map();
  const incoming = new Map();
  for (const stage of stages) for (const door of stage.doors) {
    const edge = `${stage.scene}:${door.box}`;
    if (edge === locked || failed.has(edge)) continue;
    if (!incoming.has(door.scene)) incoming.set(door.scene, []);
    incoming.get(door.scene).push(stage.scene);
  }
  const distances = new Map([[goal, 0]]), pending = [goal];
  for (const room of pending) {
    const hops = distances.get(room) + 1;
    for (const from of incoming.get(room) ?? []) {
      if (distances.has(from)) continue;
      distances.set(from, hops);
      pending.push(from);
    }
  }
  return distances;
}
