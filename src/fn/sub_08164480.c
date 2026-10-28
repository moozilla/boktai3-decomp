#include "global.h"
#include "engine_globals.h"

// MATCHING_CONCESSION: register-pinning
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: energy meter plus a 4-digit timer shown below it.
extern u8 *gGameState;
extern u16 gUnk_03005178;
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 Hud_GetStyleTileIndex(s32, s32);
s32 Div(s32, s32);

/* Tentative meter, player and game-state fields used by this display. */
struct HudMeterView {
    u8 unknown0[0xA40];
    u8 *player;
    u8 unknownA44[0x1E];
    u16 blinkTimer;
    u8 unknownA64[0x35];
    u8 timerVisible;
};
struct HudMeterPlayerView {
    u8 unknown0[0x20];
    u32 statusFlags;
    u8 unknown24[0x3F4];
    u8 kind;
    u8 unknown419[0x2B];
    u32 timerFlags;
    u8 unknown448[4];
    u32 timerFlags2;
    u8 unknown450[0x7C8];
    u8 mode;
};
struct HudMeterGameView {
    u8 unknown0[0x77C];
    s32 timer;
    u8 unknown780[0xE8];
    u32 flags;
};
#define HUD_OFFSET(member) ((u32) & ((struct HudMeterView *)0)->member)
#define HUD_FIELD(member)  (*(typeof(((struct HudMeterView *)0)->member) *)(p + HUD_OFFSET(member)))
#define PLAYER_FIELD(player, member)                                                               \
    (*(typeof(((struct HudMeterPlayerView *)0)                                                     \
                  ->member) *)((u8 *)(player) + (u32) & ((struct HudMeterPlayerView *)0)->member))
#define GAME_FIELD(member)                                                                         \
    (*(typeof(((struct HudMeterGameView *)0)->member) *)(gGameState + (u32) &                      \
                                                         ((struct HudMeterGameView *)0)->member))

static inline u32 TestBits(u32 v, u32 mask)
{
    return v & mask;
}

static inline u8 *Hud_ReadPlayer(u8 *p, u16 offset)
{
    register u32 address asm("r0") = offset;
    address += (u32)p;
    return *(u8 *volatile *)address;
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
        u8 *player = HUD_FIELD(player);
        u16 *c;
        kind = PLAYER_FIELD(player, kind);
        color = 0x1f;
        if (kind != 6)
            color = 0x35f;
        if (mode == 0) {
            fullTile = 0x53;
            emptyTile = 0x4b;
            endTile = 0x4c;
            m = 0x60;
            if (PLAYER_FIELD(player, statusFlags) & m) {
                u16 *timer = &HUD_FIELD(blinkTimer);
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
    if (((!(gEngineFlags & 0x1000)) && (!(GAME_FIELD(flags) & 0x80))) && (mode == 0)) {
        {
            u8 *player = Hud_ReadPlayer(p, HUD_OFFSET(player));
            if (player && TestBits(0x40, PLAYER_FIELD(player, timerFlags)))
                goto digits;
        }
        {
            u8 *player = Hud_ReadPlayer(p, HUD_OFFSET(player));
            if (player && TestBits(0x10, PLAYER_FIELD(player, timerFlags2)))
                goto digits;
        }
        if (PLAYER_FIELD(Hud_ReadPlayer(p, HUD_OFFSET(player)), mode) == 7) {
        digits:
            digits[0] = Div(GAME_FIELD(timer), 1000);

            rem = (GAME_FIELD(timer)) - (digits[0] * 1000);
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

            HUD_FIELD(timerVisible) = 1;
        } else if (HUD_FIELD(timerVisible)) {
            u16 *blank = Hud_GetTilemapCell(0, 9, 4);
            s32 count;
            for (count = 0; count < 4; count++)
                *(blank++) = 0xD000;

            HUD_FIELD(timerVisible) = 0;
        }
    }
}
