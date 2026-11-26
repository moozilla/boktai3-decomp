#include "player/player_barrier/scene_marker.h"
#include "engine/terrain/terrain_cell_probe.h"

s32 PlayerBarrier_TryPlace(void *task, struct SceneVec *source)
{
    struct SceneMarker *marker = task;
    struct SceneVec drawPosition;
    struct SceneVec *position;
    s32 x, y, z;
    s32 cell = cellIndexAt((struct Pos16 *)source);
    u32 flags = ((struct SceneCell *)cellProps(cell))->flags;

    if (flags & 0x1083)
        return 0;
    if (TerrainModifiers_FindCellOverride(cellIndexAt((struct Pos16 *)source), 0))
        return 0;

    x = source->x & 0xFF00;
    y = source->y;
    z = source->z & 0xFF00;
    position = &drawPosition;
    x += 0x16;
    z += 0x16;
    position->x = x;
    position->y = y;
    position->z = z;

    marker->collider.position.x = (source->x & 0xFF00) + 0x80;
    marker->collider.position.y = source->y;
    marker->collider.position.z = (source->z & 0xFF00) + 0x80;
    marker->colliderState = 1;

    marker->body.position = drawPosition;
    marker->primary.position = drawPosition;
    marker->secondary.position = drawPosition;
    marker->secondary.position.x -= 0x40;
    marker->secondary.position.z -= 0x40;
    marker->primary.flags &= ~1;
    AnimData_SelectPaletteBank(&marker->secondaryData, 0x65);
    marker->primary.mode = 2;
    return 1;
}
