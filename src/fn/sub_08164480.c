#include "global.h"

// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 89.8%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: energy meter plus a 4-digit timer shown below it.
// Remaining differences: register choice for the tile cursor and repeated player-pointer reloads in the timer test.
extern u8 *gGameState;
extern u32 gEngineFlags;
extern u16 gUnk_03005178;
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 Hud_GetStyleTileIndex(s32, s32);
s32 Div(s32, s32);

static inline u32 TestBits(u32 v, u32 mask)
{
    return v & mask;
}

#define PLAYER (*(u8 **)(p + 0xa40))

static inline u32 Arg1(u32 k_, u8 *p)
{
    return p + k_;
}

inline static u32 Arg2(u32 k_, u8 *p)
{
    return p + k_;
}

void sub_08164480(u8 *p, s32 mode, s32 row, s32 col, s32 value, s32 max, s32 cap)
{
    s32 units;
    s32 full;
    s32 partial;
    s32 n;
    s32 i;
    s32 fullTile;
    s32 emptyTile;
    s32 endTile;
    s32 color;
    u16 *cell;
    s32 digits[4];
    s32 *dp;
    s32 rem;
    s32 kind;
    u32 m;
    if (value == 0) {
        units = 0;
        full = 0;
        partial = 0;
    } else {
        units = Div((value - 1) * 79, max - 1) + 1;
        full = units >> 3;
        partial = units - (full << 3);
    }
    {
        u8 *player = *((u8 **)(p + 0xa40));
        u16 *c;
        kind = *(player + 0x418);
        color = 0x1f;
        if (kind != 6)
            color = 0x35f;
        if (mode == 0) {
            fullTile = 0x53;
            emptyTile = 0x4b;
            endTile = 0x4c;
            m = 0x60;
            if ((*((u32 *)(player + 0x20))) & m) {
                u16 *timer = (u16 *)(p + 0xa62);
                s32 bit;
                ++(*timer);
                bit = ((*timer) >> 2) & 1;
                if (bit == 0)
                    color = 0x7fff;
            }
            if (cap == 0)
                c = Hud_GetTilemapCell(0, row, col);
            else
                c = Hud_GetTilemapCell(0, row - 1, col);
        } else {
            fullTile = 0x73;
            emptyTile = 0x6b;
            endTile = 0x6c;
            if (cap != 0)
                c = Hud_GetTilemapCell(0, row - 2, col);
            else
                c = Hud_GetTilemapCell(0, row - 1, col);
        }
        if (gUnk_03005178 != color)
            gUnk_03005178 = color;
        if ((mode != 0) && (cap != 0))
            *(c++) = endTile | 0xd400;
        for (n = 0; n < full; n++)
            *(c++) = fullTile | 0xd000;

        if (partial) {
            s32 r = partial - 8;
            *(c++) = (fullTile - r) | 0xd000;
            n++;
        }
        for (; n < 10; n++)
            *(c++) = emptyTile | 0xd000;

        if (cap != 0)
            *c = endTile | 0xd000;
    }
    if (((!(gEngineFlags & 0x1000)) && (!((*((u32 *)(gGameState + 0x868))) & 0x80))) &&
        (mode == 0)) {
        if ((*((u8 **)(p + 0xa40))) && ((*((u32 *)((*((u8 **)(p + 0xa40))) + 0x444))) & 0x40))
            goto digits;
        if ((*((u8 **)(p + 0xa40))) && ((*((u32 *)((*((u8 **)Arg2(0xa40, p))) + 0x44c))) & 0x10))
            goto digits;
        if ((*((u8 *)((*((u8 **)Arg1(0xa40, p))) + 0xc18))) == 7) {
        digits:
            digits[0] = Div(*((s32 *)(gGameState + 0x77c)), 1000);

            rem = (*((s32 *)(gGameState + 0x77c))) - (digits[0] * 1000);
            digits[1] = Div(rem, 100);
            rem -= digits[1] * 100;
            digits[2] = Div(rem, 10);
            rem -= digits[2] * 10;
            digits[3] = rem;
            cell = Hud_GetTilemapCell(0, 9, 4);
            dp = digits;
            for (i = 0; i < 4; i++) {
                *(cell++) = Hud_GetStyleTileIndex(*(dp++), 0) | (-0x3000);
            }

            p[0xa99] = 1;
        } else if (p[0xa99]) {
            cell = Hud_GetTilemapCell(0, 9, 4);
            for (i = 3; i >= 0; i--)
                *(cell++) = 0xd000;

            p[0xa99] = 0;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08164480);
#endif
