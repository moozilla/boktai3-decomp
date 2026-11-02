#include "scene_hud_display.h"
#include "game_state.h"
#include "player.h"

extern u16 gUnk_03005178;
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 Hud_GetStyleTileIndex(s32, s32);

static inline u32 HasEnergyTimerBonus(struct Player *const *field)
{
    struct Player *owner = *field;
    u32 *flags, mask;
    if (!owner)
        return 0;
    flags = &owner->statBonusFlags;
    mask = 0x40;
    return *flags & mask;
}
static inline u32 HasEnergyTimerAction(struct Player *const *field)
{
    struct Player *owner = *field;
    u32 *flags, mask;
    if (!owner)
        return 0;
    flags = &owner->actionStateFlags;
    mask = 0x10;
    return *flags & mask;
}
void sub_08164480(struct SceneHudState *hud, s32 mode, s32 row, s32 col, s32 value, s32 max,
                  s32 cap)
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
    s32 remainder;
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
        struct Player *player = hud->actionOwner;
        u16 *c;
        kind = player->actionMode;
        color = 0x1f;
        if (kind != 6)
            color = 0x35f;
        if (mode == 0) {
            fullTile = 0x53;
            emptyTile = 0x4b;
            endTile = 0x4c;
            m = 0x60;
            if (player->statusFlags & m) {
                u16 *timer = &hud->energyMeterBlinkTimer;
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
    if (!(gEngineFlags & 0x1000) && !(gGameState->flags868 & 0x80) && mode == 0) {
        if (HasEnergyTimerBonus(&hud->actionOwner) || HasEnergyTimerAction(&hud->actionOwner) ||
            hud->actionOwner->statModifierContext.statBOverrideMode == 7) {
            digits[0] = Div(gGameState->unk77C, 1000);

            remainder = (gGameState->unk77C) - (digits[0] * 1000);
            digits[1] = Div(remainder, 100);
            remainder -= digits[1] * 100;
            digits[2] = Div(remainder, 10);
            remainder -= digits[2] * 10;
            digits[3] = remainder;
            cell = Hud_GetTilemapCell(0, 9, 4);
            dp = digits;
            for (i = 0; i < 4; i++) {
                *(cell++) = Hud_GetStyleTileIndex(*(dp++), 0) | (-0x3000);
            }

            hud->energyTimerVisible = 1;
        } else if (hud->energyTimerVisible) {
            u16 *blank = Hud_GetTilemapCell(0, 9, 4);
            s32 count;
            for (count = 0; count < 4; count++)
                *(blank++) = 0xD000;

            hud->energyTimerVisible = 0;
        }
    }
}
