#include "global.h"
#include "engine.h"

struct Pos {
    u16 x, pad2, z;
};
struct NavRegion {
    u8 data[16];
};
struct NavMesh {
    u16 count, unknown2;
    u32 unknown4, regionOffset;
};
struct NavResource {
    u16 count, unknown2;
    u32 offsets[1];
};
struct NavQuery {
    u8 pad[4];
    u16 mesh;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
u32 NavMesh_IsPointInsideRegion(struct NavRegion *, struct Pos *, u32);

u32 Nav_IsPositionInMesh(struct NavQuery *q, void *unused, struct Pos *pos)
{
    struct NavResource *resource = gTerrainMap->navigation;
    u32 *new_var;
    s32 x = (s16)pos->x >> 8;
    s32 z = (s16)pos->z >> 8;
    s32 cell;
    u8 *props;
    u32 mask, flags;
    s32 meshIndex;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    props = TerrainModifiers_FindCellOverride(cell, 1);
    if (props)
        props += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        props = map->cells + offset;
    }
    flags = *(u16 *)(props + 2);
    mask = 2;
    if (flags & mask)
        return 0;
    for (meshIndex = 0; meshIndex < resource->count; meshIndex++) {
        u32 sh = meshIndex << 2;
        struct NavMesh *mesh =
            (void *)((u8 *)resource + *(u32 *)((u32)(new_var = resource->offsets) + sh));
        struct NavRegion *regions = (void *)((u8 *)mesh + mesh->regionOffset);
        s32 i;
        for (i = 0; i < mesh->count; i++) {
            if (NavMesh_IsPointInsideRegion(regions, pos, i)) {
                if (meshIndex == q->mesh)
                    return 1;
                return 0;
            }
        }
    }
    return 0;
}
