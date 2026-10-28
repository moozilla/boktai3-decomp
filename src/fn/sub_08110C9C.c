#include "global.h"
#include "render.h"
#ifdef NONMATCHING
// fuzzy: 89.4%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Scrolls the tilemap panel horizontally and draws the newly exposed 8px column(s).
// Remaining: original keeps case 0/2 zeros as literals, sets offset (r1) by movs and
// tail-merges every DrawRegion call into one bl; here the cases keep separate calls.
struct TilemapPanel {
    u8 pad0[0x3c];
    s32 atEnd;
    s32 drawnX;
    s32 scrollX;
    s32 scrollY;
    u8 pad4c[8];
    s32 width;
};
struct BgScroll {
    u8 pad[0x80];
    u16 x;
};
s32 Mod(s32, s32);
void TilemapPanel_DrawRegion(struct TilemapPanel *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                             s32);
static inline u32 Arg1(u32 k_, s32 x0)
{
    return x0 + k_;
}

void sub_08110C9C(struct TilemapPanel *p)
{
    s32 *scroll = &p->scrollX;
    s32 offset;
    s32 top;
    s32 sx;
    s32 right;
    s32 limit;
    s32 diff;
    s32 next;
    s32 x0;
    s32 col;
    s32 cellWidth;
    sx = Mod(p->scrollX, 256);
    Mod(scroll[1], 256);
    (*(struct BgScroll *)gBgRecords).x = sx;
    limit = scroll[4] - 12;
    right = p->scrollX + 240;
    diff = p->drawnX - right;
    if (diff < 8) {
        next = p->drawnX + 7;
        if ((p->scrollX >> 5) > limit) {
            next = p->drawnX;
            p->atEnd = 1;
        }
    } else
        next = p->drawnX;
    if (next == p->drawnX)
        return;
    x0 = next >> 5;
    col = (next & 0xff) >> 3;
    if (diff < 0) {
        cellWidth = 2;
        next += 9;
    } else
        cellWidth = 1;
    switch (col & 3) {
        case 0:
            offset = 0;
            top = 0;
            TilemapPanel_DrawRegion(p, col, 0, x0, top, x0 + 1, 1, cellWidth, 20, offset, top);
            break;

        case 1:
            offset = 1;
            top = 0;
            TilemapPanel_DrawRegion(p, col, 0, x0, top, x0 + 1, 1, cellWidth, 20, offset, top);
            break;

        case 2:
            offset = 2;
            top = 0;
            TilemapPanel_DrawRegion(p, col, 0, x0, top, Arg1(1, x0), 1, cellWidth, 20, offset, top);
            break;

        case 3:
            offset = 3;
            top = 0;
            if (cellWidth == 1)
                TilemapPanel_DrawRegion(p, col, 0, x0, top, x0 + 1, 1, cellWidth, 20, offset, top);
            else {
                do {
                    cellWidth = 1;
                } while (0);
                TilemapPanel_DrawRegion(p, col, 0, x0, top, x0 + 1, 1, cellWidth, 20, offset, top);
                x0++;
                TilemapPanel_DrawRegion(p, (col + 1) & 31, 0, x0, top, x0 + 1, 1, cellWidth, 20,
                                        top, top);
            }
            break;
    }

    p->drawnX = (next >> 3) << 3;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08110C9C);
#endif
