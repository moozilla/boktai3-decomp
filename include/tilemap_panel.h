#ifndef GUARD_TILEMAP_PANEL_H
#define GUARD_TILEMAP_PANEL_H
#include "engine.h"

struct TilemapScrollSource {
    s32 x;
    u32 state[3];
};
struct TilemapScroll {
    s32 x, y, previousX, previousY, width, height;
};
struct TilemapPanel {
    struct Task task;
    struct TilemapScrollSource *scrollSource;
    const u16 *tileData;
    u8 *mapScript;
    u8 *map;
    s32 mapSize;
    u8 *tileListScript;
    u16 *tiles;
    s32 tileCount;
    s32 value38;
    s32 atEnd, drawnX;
    struct TilemapScroll scroll;
    u16 *screenMap;
};
void TilemapPanel_DrawRegion(struct TilemapPanel *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                             s32);
void TilemapPanel_StreamColumns(struct TilemapPanel *);
#endif
