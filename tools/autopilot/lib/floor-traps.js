// FloorTrap_TickIdle and FloorTrap_ApplyContactEffects check the player in
// the square 128 units from the trigger's center on each horizontal axis.
// SceneActorQuery_FindPlayerInBounds uses X and Z here.
exports.floorTrapCells = () => {
  const manager = $.gTileTriggerManager;
  const cells = new Set();
  if (!manager) return cells;
  for (let i = 0; i < manager.tiles.length; i++) {
    if (!(manager.used & (1 << i))) continue;
    const tile = manager.tiles[i];
    // The manager skips disabled slots, and StepHold waits for its hold flag.
    if (!tile || (tile.triggerFlags & 0x200) || !tile.state ||
        (tile.state === 3 && !(tile.triggerFlags & 0x20))) continue;
    const { x, z } = tile.position;
    // Use cell interiors. A centered trap covers one cell, and a trap off
    // center can overlap several. Pathfinding keeps turns inside safe cells.
    for (let cz = (z - 128) >> 8; cz <= (z + 127) >> 8; cz++) {
      for (let cx = (x - 128) >> 8; cx <= (x + 127) >> 8; cx++) cells.add(`${cx},${cz}`);
    }
  }
  return cells;
};
