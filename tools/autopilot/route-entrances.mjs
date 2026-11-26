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
