#include "global.h"
#ifdef NONMATCHING
// fuzzy: 85.9%
// fix.py best (sweep after the inlined-insn fix)
// Remaining: the 0xFFFFD000 tile attribute mask is copied from the literal pool through a second register in the original.
s32 Div(s32, s32);
u16 *Hud_GetTilemapCell(s32, s32, s32);
extern u16 gUnk_03005178;
void StatGauge_Draw(s32 style, s32 x, s32 y, s32 current, s32 maximum, s32 attr)
{
    s32 whole;
    s32 partial;
    u32 filled;
    u32 empty;
    u16 *out;
    s32 i;
    if (current == 0) {
        whole = 0;
        partial = 0;
    } else {
        s32 units = Div((current - 1) * 79, maximum - 1) + 1;
        whole = units >> 3;
        partial = units - (whole << 3);
    }
    if (style == 0) {
        filled = 0x53;
        empty = 0x4B;
        out = Hud_GetTilemapCell(0, x, y);
    } else {
        filled = 0x73;
        empty = 0x6B;
        out = Hud_GetTilemapCell(0, x - 1, y);
    }
    if (gUnk_03005178 != attr)
        gUnk_03005178 = attr;
    for (i = 0; i < whole; i++) {
        s32 mask = -12288;
        *(out++) = filled | mask;
    }

    if (partial) {
        s16 mask = -12288;
        s32 diff = partial;
        diff -= 8;
        diff = filled - diff;
        *(out++) = diff | mask;
        i++;
    }
    while (i <= 9) {
        s32 mask = -12288;
        *(out++) = empty | mask;
        i++;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", StatGauge_Draw);
#endif
