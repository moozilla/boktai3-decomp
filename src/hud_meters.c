#include "hud.h"
extern u16 gUnk_03005178;

void Hud_DrawHealthMeterCells(struct SceneHudState *p, s32 mode, s32 row, s32 col, s32 cap,
                              s32 value, s32 max)
{
    s32 units, full, partial, n;
    s32 fullTile, emptyTile, endTile;
    s32 color;
    u16 *cell;
    if (value == 0) {
        units = 0;
        full = 0;
        partial = 0;
    } else {
        units = Div((value - 1) * 79, max - 1) + 1;
        full = units >> 3;
        partial = units - (full << 3);
    }
    if (mode == 0) {
        fullTile = 0x43;
        emptyTile = 0x4b;
        endTile = 0x4c;
        if (units > 39) {
            color = 0x3e0;
        } else if (units > 19) {
            color = 0x29f;
        } else {
            u16 *timer = &p->healthMeterBlinkTimer;
            u16 t = *timer;
            s32 bit = (t >> 2) & 1;
            color = 0x12;
            if (bit)
                color = 0x109f;
            *timer = t + 1;
        }
        if (cap == 0)
            cell = Hud_GetTilemapCell(0, row, col);
        else
            cell = Hud_GetTilemapCell(0, row - 1, col);
    } else {
        fullTile = 0x63;
        emptyTile = 0x6b;
        endTile = 0x6c;
        color = 0x3e0;
        if (units <= 39) {
            color = 0x109f;
            if (units > 19)
                color = 0x29f;
        }
        if (cap != 0)
            cell = Hud_GetTilemapCell(0, row - 2, col);
        else
            cell = Hud_GetTilemapCell(0, row - 1, col);
    }
    if (gBgPaletteBank13Color11 != color)
        gBgPaletteBank13Color11 = color;
    if (mode != 0 && cap != 0)
        *cell++ = endTile | 0xd400;
    for (n = 0; n < full; n++)
        *cell++ = fullTile | 0xd000;
    if (partial) {
        s32 r = partial - 8;
        *cell++ = (fullTile - r) | 0xd000;
        n++;
    }
    for (; n < 10; n++)
        *cell++ = emptyTile | 0xd000;
    if (cap != 0)
        *cell = endTile | 0xd000;
}

void Hud_DrawHealthMeter(struct SceneHudState *p, u32 a, u32 b, u32 c, u32 d)
{
    Hud_DrawHealthMeterCells(p, a, b, c, d, p->actionOwner->currentStatA,
                             p->actionOwner->maximumStatA);
}

void Hud_DrawDefaultHealthMeter(struct SceneHudState *a, u32 b, u32 c, u32 d)
{
    Hud_DrawHealthMeter(a, b, c, d, 0);
}

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

void Hud_DrawEnergyMeter(struct SceneHudState *hud, s32 mode, s32 row, s32 col, s32 value, s32 max,
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

void Hud_DrawDefaultEnergyMeter(struct SceneHudState *a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    Hud_DrawEnergyMeter(a, b, c, d, e, f, 0);
}

void Hud_DrawHealthLabel(s32 unused, s32 x, s32 y)
{
    u16 *p = Hud_GetTilemapCell(0, x, y);
    *p++ = 0xD040;
    *p++ = 0xD041;
    *p = 0xD042;
}

void Hud_DrawEnergyLabel(s32 unused, s32 x, s32 y)
{
    u16 *p = Hud_GetTilemapCell(0, x, y);
    *p++ = 0xD050;
    *p++ = 0xD051;
    *p = 0xD052;
}

void Hud_DrawAlternateMeterLabel(s32 unused, s32 x, s32 y)
{
    u16 *p = Hud_GetTilemapCell(0, x, y);
    *p++ = 0xD08D;
    *p++ = 0xD08E;
    *p = 0xD08F;
}

void Hud_DrawPlayerGauge(struct SceneHudState *p, s32 x, s32 y)
{
    s32 filled, i;
    u16 *tiles;
    if (!p->playerValueEnabled) {
        tiles = Hud_GetTilemapCell(0, x - 3, y);
        i = 0;
        do {
            *tiles++ = 0xD000;
        } while (++i < 14);
    } else {
        if ((u8)(p->actionOwner->actionMode - 1) <= 1) {
            if (p->actionOwner->statGaugeFull == 1)
                filled = 10;
            else
                filled = Div(p->actionOwner->statusValue, 400);
        } else
            filled = p->actionOwner->statWhole;
        tiles = Hud_GetTilemapCell(0, x, y);
        for (i = 0; i < filled; i++)
            *tiles++ = 0xD05C;
        for (; i < 10; i++)
            *tiles++ = 0xD05E;
    }
}
