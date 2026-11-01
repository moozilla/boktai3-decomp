#ifndef GUARD_TILEMAP_PANEL_H
#define GUARD_TILEMAP_PANEL_H
#include "global.h"

struct TilemapScroll {
    s32 x, y, previousX, previousY, width, height;
};
struct TilemapPanel {
    u8 task[0x18];
    s32 *scrollSource;
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
void sub_08110C9C(struct TilemapPanel *);
#endif
