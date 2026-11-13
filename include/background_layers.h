#ifndef GUARD_BACKGROUND_LAYERS_H
#define GUARD_BACKGROUND_LAYERS_H
#include "render.h"
#include "interrupts_video.h"
struct BgTileEntry {
    u16 id, count;
    u32 offset;
};
struct BgTileResource {
    u16 count;
    u8 unknown02[10];
    u32 entries, tileIndices, tiles;
};
struct BgMapResource {
    u8 signature[2];
    u16 unknown02, unknown04, count, unknown08, tileCount;
    u32 maps, tiles, extra;
};
struct BgMapEntry {
    u16 width, height;
    u32 unknown04, data;
};
struct BgLayerResource {
    u32 unknown00[3];
    u32 mapRows;
    s16 tileCount, tileIndex;
    u32 tiles;
    u32 unknown18;
    u32 extraData;
    u32 unknown20;
    u32 mapData;
    u32 unknown28, rowWidth, unknown30, unknown34;
    u32 width, height;
};
#endif
