#include "scene_object.h"
#include "terrain_cell_probe.h"

struct SceneMarkerCollider {
    u8 unknown0[0xC];
    struct SceneVec position;
    u8 unknown14[0x40];
};
struct SceneMarkerSprite {
    u32 flags;
    u8 unknown4[3];
    u8 mode;
    u8 unknown8[0x14];
    struct SceneVec position;
    u8 unknown24[8];
};
struct SceneMarker {
    u8 task[0x18];
    struct SceneBody body;
    u32 source;
    struct SceneMarkerCollider collider;
    u32 colliderState;
    struct SceneAnimation secondaryData, primaryData;
    struct SceneMarkerSprite primary;
    struct SceneMarkerSprite secondary;
    u32 state;
};

s32 PlayerBarrier_TryPlace(void *task, struct SceneVec *source)
{
    struct SceneMarker *marker = task;
    struct SceneVec drawPosition;
    struct SceneVec *position;
    s64 cellMask;
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

    cellMask = 0xFFFFFF00;
    marker->collider.position.x = ((u16)source->x & cellMask) + 0x80;
    marker->collider.position.y = source->y;
    marker->collider.position.z = ((u16)source->z & cellMask) + 0x80;
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
