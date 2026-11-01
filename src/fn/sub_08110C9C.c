#include "tilemap_panel.h"
#include "render.h"
s32 Mod(s32, s32);

void TilemapPanel_StreamColumns(struct TilemapPanel *p)
{
    struct TilemapScroll *scroll = &p->scroll;
    s32 stripOffset;
    s32 firstRow;
    s32 scrollX;
    s32 visibleRight;
    s32 lastTile;
    s32 distance;
    s32 rightEdge;
    s32 tileX;
    s32 cursorX;
    s32 column;
    s32 cellWidth;
    scrollX = Mod(p->scroll.x, 256);
    Mod(scroll->y, 256);
    gBgRecords[2].x = scrollX;
    lastTile = scroll->width - 12;
    visibleRight = p->scroll.x + 240;
    stripOffset = p->drawnX;
    distance = stripOffset - visibleRight;
    if (distance < 8) {
        tileX = p->scroll.x >> 5;
        rightEdge = p->drawnX + 7;
        if (tileX > lastTile) {
            rightEdge = p->drawnX;
            p->atEnd = 1;
        }
    } else
        rightEdge = p->drawnX;
    if (rightEdge == p->drawnX)
        return;
    cursorX = rightEdge >> 5;
    column = (rightEdge & 0xff) >> 3;
    if (distance < 0) {
        cellWidth = 2;
        rightEdge += 9;
    } else
        cellWidth = 1;
    switch (column & 3) {
        case 0:
            stripOffset = 0;
            firstRow = 0;
            TilemapPanel_DrawRegion(p, column, 0, cursorX, firstRow, cursorX + 1, 1, cellWidth, 20,
                                    stripOffset, firstRow);
            cursorX = rightEdge;
            break;

        case 1:
            stripOffset = 1;
            firstRow = 0;
            TilemapPanel_DrawRegion(p, column, 0, cursorX, firstRow, cursorX + 1, 1, cellWidth, 20,
                                    stripOffset, firstRow);
            cursorX = rightEdge;
            break;

        case 2:
            stripOffset = 2;
            firstRow = 0;
            TilemapPanel_DrawRegion(p, column, firstRow, cursorX, firstRow, cursorX + 1, 1,
                                    cellWidth, 20, stripOffset, firstRow);
            cursorX = rightEdge;
            break;

        case 3:
            stripOffset = 3;
            firstRow = 0;
            if (cellWidth == 1) {
                TilemapPanel_DrawRegion(p, column, 0, cursorX, firstRow, cursorX + 1, 1, cellWidth,
                                        20, stripOffset, firstRow);
                cursorX = rightEdge;
            } else {
                cellWidth = 1;
                TilemapPanel_DrawRegion(p, column, 0, cursorX, firstRow, cursorX + 1, 1, cellWidth,
                                        20, stripOffset, firstRow);
                cursorX++;
                TilemapPanel_DrawRegion(p, (column + 1) & 31, 0, cursorX, firstRow, cursorX + 1, 1,
                                        cellWidth, 20, firstRow, firstRow);
                cursorX = rightEdge;
            }
            break;
        default:
            cursorX = rightEdge;
            break;
    }

    p->drawnX = (cursorX >> 3) << 3;
}
