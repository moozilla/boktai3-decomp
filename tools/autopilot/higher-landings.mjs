// The destination entrance identifies the landing, even when two doors in
// the same scene lead to different floors. Use the entrance's HP box height.
export function landingReturn(stages, room, door, floor, distanceTo) {
  const direction = Math.sign(door.y - floor);
  const heights = new Map(room.doors.map(({ box, y }) => [box, y]));
  const entrances = new Map(room.entrances.map(({ id, box }) => [id, heights.get(box)]));
  let route = null;
  for (const stage of stages) {
    if (stage.scene === room.scene) continue;
    for (const exit of stage.doors) {
      if (exit.scene !== room.scene) continue;
      const height = entrances.get(exit.entrance);
      if (!((height - floor) * direction > 0 && (door.y - height) * direction >= 0)) continue;
      const distance = distanceTo(stage.scene).get(room.scene);
      if (distance === undefined || (route && distance >= route.distance)) continue;
      route = { room: room.scene, box: door.box, via: stage.scene, returnBox: exit.box,
        entrance: exit.entrance, height, distance, ...(direction < 0 ? { descending: true } : {}) };
    }
  }
  return route;
}
