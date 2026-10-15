#ifndef GUARD_TERRAIN_EDGE_MARKER_H
#define GUARD_TERRAIN_EDGE_MARKER_H
#include "scene_object.h"
/* Tentative: directional marker selected from a region's terrain edges. */
union TerrainMarkerPosition {
    struct SceneVec world;
    u32 words[2];
    struct {
        u32 x : 16;
        u32 y : 16;
        u32 z : 16;
        u32 pad : 16;
    } packed;
};
struct TerrainMarkerSprite {
    u32 flags;
    u8 unknown4[0xB], renderMode, unknown10[8];
    union TerrainMarkerPosition position;
    u8 unknown20[8];
};
struct TerrainMarkerManager {
    u8 task[0x18];
    u16 mode, unknown1A, count, blinkFrame;
    struct TerrainMarkerSprite markers[17];
};
struct TerrainMarkerRegion {
    s16 minX, minZ, maxX, maxZ;
    u16 unknown8, tag;
};
struct TerrainMarkerManager *sub_0821A2C4(u16);
void sub_080560C0(union TerrainMarkerPosition *, s32, s32, s32);
s32 sub_08056298(struct TerrainMarkerRegion *);
s32 sub_08056954(struct TerrainMarkerManager *);
s32 sub_080569D8(struct TerrainMarkerManager *);
s32 sub_08056A00(struct TerrainMarkerManager *);
#endif
