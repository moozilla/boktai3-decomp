#include "global.h"
#include "input.h"
void MapMenu_ClampScrollAndUpdateArrows(u8 *);
void BgGfx_LoadTilemapSimple(s32, s32, s32, s32, s32);
void MapMenu_DrawLocationIcons(u8 *, s32, s32);
void MapScreen_StepDirectionalIndicators(u8 *);
#define F16(p, o) (*(s16 *)((u8 *)(p) + (o)))
void sub_0816CB50(u8 *p)
{
    s16 *x;
    s16 *y;
    s32 moved;
    moved = 0;
    if ((*(u16 *)gPadState) & 0x20) {
        F16(p, 0x490C) -= 2;
        moved = 1;
    } else if ((*(u16 *)gPadState) & 0x10) {
        F16(p, 0x490C) += 2;
        moved = 1;
    }
    if ((*(u16 *)gPadState) & 0x40) {
        F16(p, 0x490E) -= 2;
        moved = 1;
    } else if ((*(u16 *)gPadState) & 0x80) {
        F16(p, 0x490E) += 2;
        moved = 1;
    }
    if (moved) {
        MapMenu_ClampScrollAndUpdateArrows(p);
        x = &F16(p, 0x490C);
        BgGfx_LoadTilemapSimple(1, (s16)*x >> 3, ((s16) * (y = &F16(p, 0x490E)) >> 3) & 0xFFFFFFFE,
                                *x, *y);

        MapMenu_DrawLocationIcons(p, (s16)*x >> 3, ((s16)*y >> 3) & 0xFFFFFFFE);
        {
            s16 v = F16(p, 0x491E) - *x;
            F16(p, 0x3840) = v;
        }
        {
            s16 v = F16(p, 0x4920) - *y;
            F16(p, 0x3842) = v;
        }
        if (!(*(u32 *)(p + 0x3888) & 1)) {
            {
                s16 v = F16(p, 0x4924) - *x;
                F16(p, 0x38A0) = v;
            }
            {
                s16 v = F16(p, 0x4926) - *y;
                F16(p, 0x38A2) = v;
            }
        }
    }
    MapScreen_StepDirectionalIndicators(p);
}
