#include "global.h"
struct TilemapPanel {
    u8 pad0[0x1c];
    const u16 *tileData;
    u8 *mapScript;
    u8 *map;
    s32 mapSize;
    u8 *tileListScript;
    u16 *tiles;
    s32 tileCount;
    s32 value38;
    u8 pad3c[4];
    s32 value40;
    u8 pad44[0x10];
    s32 width, height;
    u16 *screenMap;
};
void TileRows_CopyPackedRows(u16 *dest, s32 page, s32 offset, s32 firstRow, s32 width, s32 height,
                             const u16 *source);
void TilemapPanel_DrawRegion(struct TilemapPanel *p, s32 col, s32 row, s32 x0, s32 y0, s32 x1,
                             s32 y1, s32 cellWidth, s32 cellHeight, s32 offset, s32 firstRow)
{
    s32 base = col + row * 128;
    s32 index = base;
    u16 *screen = p->screenMap;
    s32 x, y;
    for (y = y0; y < y1; y++) {
        s32 rowOff = p->width * y;
        for (x = x0; x < x1; x++) {
            TileRows_CopyPackedRows(&screen[index], p->map[x + rowOff], offset, firstRow, cellWidth,
                                    cellHeight, p->tileData);
            index += cellWidth;
        }
        index = base + (y + 1) * 128;
    }
}
