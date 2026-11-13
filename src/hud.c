#include "hud.h"
#include "sun_clock.h"
#include "link_session_state.h"
#include "sun_clock.h"

#include "sun_clock.h"
extern u16 gUnk_03005178;
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawHealthMeterCells(struct SceneHudState *hud, s32 mode, s32 x, s32 y, s32 cap, s32 value,
#include "sun_clock.h"
                              s32 maximum)
#include "sun_clock.h"
{
#include "sun_clock.h"
    s32 units, full, partial, n;
#include "sun_clock.h"
    s32 fullTile, emptyTile, endTile;
#include "sun_clock.h"
    s32 color;
#include "sun_clock.h"
    u16 *cell;
#include "sun_clock.h"
    if (value == 0) {
#include "sun_clock.h"
        units = 0;
#include "sun_clock.h"
        full = 0;
#include "sun_clock.h"
        partial = 0;
#include "sun_clock.h"
    } else {
#include "sun_clock.h"
        units = Div((value - 1) * 79, maximum - 1) + 1;
#include "sun_clock.h"
        full = units >> 3;
#include "sun_clock.h"
        partial = units - (full << 3);
#include "sun_clock.h"
    }
#include "sun_clock.h"
    if (mode == 0) {
#include "sun_clock.h"
        fullTile = 0x43;
#include "sun_clock.h"
        emptyTile = 0x4b;
#include "sun_clock.h"
        endTile = 0x4c;
#include "sun_clock.h"
        if (units > 39) {
#include "sun_clock.h"
            color = 0x3e0;
#include "sun_clock.h"
        } else if (units > 19) {
#include "sun_clock.h"
            color = 0x29f;
#include "sun_clock.h"
        } else {
#include "sun_clock.h"
            u16 *timer = &hud->healthMeterBlinkTimer;
#include "sun_clock.h"
            u16 t = *timer;
#include "sun_clock.h"
            s32 bit = (t >> 2) & 1;
#include "sun_clock.h"
            color = 0x12;
#include "sun_clock.h"
            if (bit)
#include "sun_clock.h"
                color = 0x109f;
#include "sun_clock.h"
            *timer = t + 1;
#include "sun_clock.h"
        }
#include "sun_clock.h"
        if (cap == 0)
#include "sun_clock.h"
            cell = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
        else
#include "sun_clock.h"
            cell = Hud_GetTilemapCell(0, x - 1, y);
#include "sun_clock.h"
    } else {
#include "sun_clock.h"
        fullTile = 0x63;
#include "sun_clock.h"
        emptyTile = 0x6b;
#include "sun_clock.h"
        endTile = 0x6c;
#include "sun_clock.h"
        color = 0x3e0;
#include "sun_clock.h"
        if (units <= 39) {
#include "sun_clock.h"
            color = 0x109f;
#include "sun_clock.h"
            if (units > 19)
#include "sun_clock.h"
                color = 0x29f;
#include "sun_clock.h"
        }
#include "sun_clock.h"
        if (cap != 0)
#include "sun_clock.h"
            cell = Hud_GetTilemapCell(0, x - 2, y);
#include "sun_clock.h"
        else
#include "sun_clock.h"
            cell = Hud_GetTilemapCell(0, x - 1, y);
#include "sun_clock.h"
    }
#include "sun_clock.h"
    if (gBgPaletteBank13Color11 != color)
#include "sun_clock.h"
        gBgPaletteBank13Color11 = color;
#include "sun_clock.h"
    if (mode != 0 && cap != 0)
#include "sun_clock.h"
        *cell++ = endTile | 0xd400;
#include "sun_clock.h"
    for (n = 0; n < full; n++)
#include "sun_clock.h"
        *cell++ = fullTile | 0xd000;
#include "sun_clock.h"
    if (partial) {
#include "sun_clock.h"
        s32 r = partial - 8;
#include "sun_clock.h"
        *cell++ = (fullTile - r) | 0xd000;
#include "sun_clock.h"
        n++;
#include "sun_clock.h"
    }
#include "sun_clock.h"
    for (; n < 10; n++)
#include "sun_clock.h"
        *cell++ = emptyTile | 0xd000;
#include "sun_clock.h"
    if (cap != 0)
#include "sun_clock.h"
        *cell = endTile | 0xd000;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawHealthMeter(struct SceneHudState *hud, u32 mode, u32 x, u32 y, u32 cap)
#include "sun_clock.h"
{
#include "sun_clock.h"
    Hud_DrawHealthMeterCells(hud, mode, x, y, cap, hud->actionOwner->currentStatA,
#include "sun_clock.h"
                             hud->actionOwner->maximumStatA);
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawDefaultHealthMeter(struct SceneHudState *hud, u32 mode, u32 x, u32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    Hud_DrawHealthMeter(hud, mode, x, y, 0);
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
static inline u32 HasEnergyTimerBonus(struct Player *const *field)
#include "sun_clock.h"
{
#include "sun_clock.h"
    struct Player *owner = *field;
#include "sun_clock.h"
    u32 *flags, mask;
#include "sun_clock.h"
    if (!owner)
#include "sun_clock.h"
        return 0;
#include "sun_clock.h"
    flags = &owner->statBonusFlags;
#include "sun_clock.h"
    mask = 0x40;
#include "sun_clock.h"
    return *flags & mask;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
static inline u32 HasEnergyTimerAction(struct Player *const *field)
#include "sun_clock.h"
{
#include "sun_clock.h"
    struct Player *owner = *field;
#include "sun_clock.h"
    u32 *flags, mask;
#include "sun_clock.h"
    if (!owner)
#include "sun_clock.h"
        return 0;
#include "sun_clock.h"
    flags = &owner->actionStateFlags;
#include "sun_clock.h"
    mask = 0x10;
#include "sun_clock.h"
    return *flags & mask;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawEnergyMeter(struct SceneHudState *hud, s32 mode, s32 x, s32 y, s32 value, s32 maximum,
#include "sun_clock.h"
                         s32 cap)
#include "sun_clock.h"
{
#include "sun_clock.h"
    s32 units;
#include "sun_clock.h"
    s32 full;
#include "sun_clock.h"
    s32 partial;
#include "sun_clock.h"
    s32 n;
#include "sun_clock.h"
    s32 i;
#include "sun_clock.h"
    s32 fullTile;
#include "sun_clock.h"
    s32 emptyTile;
#include "sun_clock.h"
    s32 endTile;
#include "sun_clock.h"
    s32 color;
#include "sun_clock.h"
    u16 *cell;
#include "sun_clock.h"
    s32 digits[4];
#include "sun_clock.h"
    s32 *dp;
#include "sun_clock.h"
    s32 remainder;
#include "sun_clock.h"
    s32 kind;
#include "sun_clock.h"
    u32 m;
#include "sun_clock.h"
    if (value == 0) {
#include "sun_clock.h"
        units = 0;
#include "sun_clock.h"
        full = 0;
#include "sun_clock.h"
        partial = 0;
#include "sun_clock.h"
    } else {
#include "sun_clock.h"
        units = Div((value - 1) * 79, maximum - 1) + 1;
#include "sun_clock.h"
        full = units >> 3;
#include "sun_clock.h"
        partial = units - (full << 3);
#include "sun_clock.h"
    }
#include "sun_clock.h"
    {
#include "sun_clock.h"
        struct Player *player = hud->actionOwner;
#include "sun_clock.h"
        u16 *c;
#include "sun_clock.h"
        kind = player->actionMode;
#include "sun_clock.h"
        color = 0x1f;
#include "sun_clock.h"
        if (kind != 6)
#include "sun_clock.h"
            color = 0x35f;
#include "sun_clock.h"
        if (mode == 0) {
#include "sun_clock.h"
            fullTile = 0x53;
#include "sun_clock.h"
            emptyTile = 0x4b;
#include "sun_clock.h"
            endTile = 0x4c;
#include "sun_clock.h"
            m = 0x60;
#include "sun_clock.h"
            if (player->statusFlags & m) {
#include "sun_clock.h"
                u16 *timer = &hud->energyMeterBlinkTimer;
#include "sun_clock.h"
                s32 bit;
#include "sun_clock.h"
                ++(*timer);
#include "sun_clock.h"
                bit = ((*timer) >> 2) & 1;
#include "sun_clock.h"
                if (bit == 0)
#include "sun_clock.h"
                    color = 0x7fff;
#include "sun_clock.h"
            }
#include "sun_clock.h"
            if (cap == 0)
#include "sun_clock.h"
                c = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
            else
#include "sun_clock.h"
                c = Hud_GetTilemapCell(0, x - 1, y);
#include "sun_clock.h"
        } else {
#include "sun_clock.h"
            fullTile = 0x73;
#include "sun_clock.h"
            emptyTile = 0x6b;
#include "sun_clock.h"
            endTile = 0x6c;
#include "sun_clock.h"
            if (cap != 0)
#include "sun_clock.h"
                c = Hud_GetTilemapCell(0, x - 2, y);
#include "sun_clock.h"
            else
#include "sun_clock.h"
                c = Hud_GetTilemapCell(0, x - 1, y);
#include "sun_clock.h"
        }
#include "sun_clock.h"
        if (gUnk_03005178 != color)
#include "sun_clock.h"
            gUnk_03005178 = color;
#include "sun_clock.h"
        if ((mode != 0) && (cap != 0))
#include "sun_clock.h"
            *(c++) = endTile | 0xd400;
#include "sun_clock.h"
        for (n = 0; n < full; n++)
#include "sun_clock.h"
            *(c++) = fullTile | 0xd000;
#include "sun_clock.h"

#include "sun_clock.h"
        if (partial) {
#include "sun_clock.h"
            s32 r = partial - 8;
#include "sun_clock.h"
            *(c++) = (fullTile - r) | 0xd000;
#include "sun_clock.h"
            n++;
#include "sun_clock.h"
        }
#include "sun_clock.h"
        for (; n < 10; n++)
#include "sun_clock.h"
            *(c++) = emptyTile | 0xd000;
#include "sun_clock.h"

#include "sun_clock.h"
        if (cap != 0)
#include "sun_clock.h"
            *c = endTile | 0xd000;
#include "sun_clock.h"
    }
#include "sun_clock.h"
    if (!(gEngineFlags & 0x1000) && !(gGameState->flags868 & 0x80) && mode == 0) {
#include "sun_clock.h"
        if (HasEnergyTimerBonus(&hud->actionOwner) || HasEnergyTimerAction(&hud->actionOwner) ||
#include "sun_clock.h"
            hud->actionOwner->statModifierContext.statBOverrideMode == 7) {
#include "sun_clock.h"
            digits[0] = Div(gGameState->unk77C, 1000);
#include "sun_clock.h"

#include "sun_clock.h"
            remainder = (gGameState->unk77C) - (digits[0] * 1000);
#include "sun_clock.h"
            digits[1] = Div(remainder, 100);
#include "sun_clock.h"
            remainder -= digits[1] * 100;
#include "sun_clock.h"
            digits[2] = Div(remainder, 10);
#include "sun_clock.h"
            remainder -= digits[2] * 10;
#include "sun_clock.h"
            digits[3] = remainder;
#include "sun_clock.h"
            cell = Hud_GetTilemapCell(0, 9, 4);
#include "sun_clock.h"
            dp = digits;
#include "sun_clock.h"
            for (i = 0; i < 4; i++) {
#include "sun_clock.h"
                *(cell++) = Hud_GetStyleTileIndex(*(dp++), 0) | (-0x3000);
#include "sun_clock.h"
            }
#include "sun_clock.h"

#include "sun_clock.h"
            hud->energyTimerVisible = 1;
#include "sun_clock.h"
        } else if (hud->energyTimerVisible) {
#include "sun_clock.h"
            u16 *blank = Hud_GetTilemapCell(0, 9, 4);
#include "sun_clock.h"
            s32 count;
#include "sun_clock.h"
            for (count = 0; count < 4; count++)
#include "sun_clock.h"
                *(blank++) = 0xD000;
#include "sun_clock.h"

#include "sun_clock.h"
            hud->energyTimerVisible = 0;
#include "sun_clock.h"
        }
#include "sun_clock.h"
    }
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawDefaultEnergyMeter(struct SceneHudState *hud, s32 mode, s32 x, s32 y, s32 value,
#include "sun_clock.h"
                                s32 maximum)
#include "sun_clock.h"
{
#include "sun_clock.h"
    Hud_DrawEnergyMeter(hud, mode, x, y, value, maximum, 0);
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawHealthLabel(s32 unused, s32 x, s32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u16 *cell = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
    *cell++ = 0xD040;
#include "sun_clock.h"
    *cell++ = 0xD041;
#include "sun_clock.h"
    *cell = 0xD042;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawEnergyLabel(s32 unused, s32 x, s32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u16 *cell = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
    *cell++ = 0xD050;
#include "sun_clock.h"
    *cell++ = 0xD051;
#include "sun_clock.h"
    *cell = 0xD052;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawAlternateMeterLabel(s32 unused, s32 x, s32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u16 *cell = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
    *cell++ = 0xD08D;
#include "sun_clock.h"
    *cell++ = 0xD08E;
#include "sun_clock.h"
    *cell = 0xD08F;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawPlayerGauge(struct SceneHudState *hud, s32 x, s32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    s32 filled, i;
#include "sun_clock.h"
    u16 *tiles;
#include "sun_clock.h"
    if (!hud->playerValueEnabled) {
#include "sun_clock.h"
        tiles = Hud_GetTilemapCell(0, x - 3, y);
#include "sun_clock.h"
        i = 0;
#include "sun_clock.h"
        do {
#include "sun_clock.h"
            *tiles++ = 0xD000;
#include "sun_clock.h"
        } while (++i < 14);
#include "sun_clock.h"
    } else {
#include "sun_clock.h"
        if ((u8)(hud->actionOwner->actionMode - 1) <= 1) {
#include "sun_clock.h"
            if (hud->actionOwner->statGaugeFull == 1)
#include "sun_clock.h"
                filled = 10;
#include "sun_clock.h"
            else
#include "sun_clock.h"
                filled = Div(hud->actionOwner->statusValue, 400);
#include "sun_clock.h"
        } else
#include "sun_clock.h"
            filled = hud->actionOwner->statWhole;
#include "sun_clock.h"
        tiles = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
        for (i = 0; i < filled; i++)
#include "sun_clock.h"
            *tiles++ = 0xD05C;
#include "sun_clock.h"
        for (; i < 10; i++)
#include "sun_clock.h"
            *tiles++ = 0xD05E;
#include "sun_clock.h"
    }
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
static inline s32 OwnerIndex(void)
#include "sun_clock.h"
{
#include "sun_clock.h"
    if (!gLinkSession)
#include "sun_clock.h"
        return -1;
#include "sun_clock.h"
    return gLinkSession->localPlayer;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
static inline u8 SlotCount(s32 slot)
#include "sun_clock.h"
{
#include "sun_clock.h"
    if (!gLinkSession || slot < 0)
#include "sun_clock.h"
        return 0;
#include "sun_clock.h"
    return gLinkSession->playerValues[slot];
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
static inline u32 testFlags(const u32 *flags, u32 mask)
#include "sun_clock.h"
{
#include "sun_clock.h"
    return *flags & mask;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
static inline u8 testFlag(u32 mask, u32 *flags)
#include "sun_clock.h"
{
#include "sun_clock.h"
    if (*flags & mask)
#include "sun_clock.h"
        return TRUE;
#include "sun_clock.h"
    return FALSE;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
static inline u32 LinkFlags(u32 mask)
#include "sun_clock.h"
{
#include "sun_clock.h"
    return (gEngineFlags | gScenePauseFlags) & mask;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
static inline s32 TimedEventValue(u32 slot)
#include "sun_clock.h"
{
#include "sun_clock.h"
    return ((struct GameState *)(slot * sizeof(s32) + (u32)gGameState))->counters628[0];
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawWarningMeter(struct SceneHudState *hud, s32 mode, u32 x, u32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    s32 blink;
#include "sun_clock.h"
    u16 *tiles;
#include "sun_clock.h"
    s32 i;
#include "sun_clock.h"
    u32 pulse;
#include "sun_clock.h"
    s32 count;
#include "sun_clock.h"

#include "sun_clock.h"
    if (hud->actionOwner->tetherMode == 0x23 && hud->controlMode != 8)
#include "sun_clock.h"
        goto blank;
#include "sun_clock.h"
    if (gGameState->playerParticleRate > 0x9C3 && hud->controlMode != 8) {
#include "sun_clock.h"
        u16 *timer = &hud->warningBlinkTimer;
#include "sun_clock.h"
        u32 bit = (*timer >> 4) & 1;
#include "sun_clock.h"
        blink = 1;
#include "sun_clock.h"
        if (bit)
#include "sun_clock.h"
            blink = 0;
#include "sun_clock.h"
        if ((*timer & 0x3F) == 2)
#include "sun_clock.h"
            Sound_PlaySong(0x132);
#include "sun_clock.h"
        (*timer)++;
#include "sun_clock.h"
    } else if (gGameState->playerParticleRate > 0 && hud->controlMode != 8) {
#include "sun_clock.h"
        u16 *timer = &hud->warningBlinkTimer;
#include "sun_clock.h"
        u32 bit = (*timer >> 5) & 1;
#include "sun_clock.h"
        blink = 1;
#include "sun_clock.h"
        if (bit)
#include "sun_clock.h"
            blink = 0;
#include "sun_clock.h"
        if ((*timer & 0x3F) == 2)
#include "sun_clock.h"
            Sound_PlaySong(0x131);
#include "sun_clock.h"
        (*timer)++;
#include "sun_clock.h"
    } else {
#include "sun_clock.h"
        blink = 0;
#include "sun_clock.h"
        hud->warningBlinkTimer = blink;
#include "sun_clock.h"
    }
#include "sun_clock.h"
    if (blink) {
#include "sun_clock.h"
    blank:
#include "sun_clock.h"
        tiles = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
        for (i = 0; i < 12; i++)
#include "sun_clock.h"
            *tiles++ = (i + 3) | 0xD000;
#include "sun_clock.h"
    } else {
#include "sun_clock.h"
        tiles = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
        if (mode == 1)
#include "sun_clock.h"
            pulse = 0x29F;
#include "sun_clock.h"
        else if (testFlag(0x80, &gGameState->flags868) || hud->controlMode == 8 ||
#include "sun_clock.h"
                 testFlags(&hud->actionOwner->statusFlags, 0x10)) {
#include "sun_clock.h"
            u32 frame = hud->meterPulseFrame;
#include "sun_clock.h"
            pulse = 0x29F;
#include "sun_clock.h"
            if (frame > 0x24) {
#include "sun_clock.h"
                pulse = 0x1FB;
#include "sun_clock.h"
                if (frame <= 0x2C)
#include "sun_clock.h"
                    goto pulse_ready;
#include "sun_clock.h"
                if (frame <= 0x34)
#include "sun_clock.h"
                    pulse = 0x177;
#include "sun_clock.h"
            }
#include "sun_clock.h"
        pulse_ready:
#include "sun_clock.h"
            if (++hud->meterPulseFrame > 0x3B)
#include "sun_clock.h"
                hud->meterPulseFrame = 0;
#include "sun_clock.h"
        } else {
#include "sun_clock.h"
            hud->meterPulseFrame = 0x34;
#include "sun_clock.h"
            pulse = 0x177;
#include "sun_clock.h"
        }
#include "sun_clock.h"
        if (gBgPaletteBank13Color10 != pulse)
#include "sun_clock.h"
            gBgPaletteBank13Color10 = pulse;
#include "sun_clock.h"
        if (LinkFlags(0x800))
#include "sun_clock.h"
            count = SlotCount(OwnerIndex());
#include "sun_clock.h"
        else
#include "sun_clock.h"
            count = gGameState->unk876;
#include "sun_clock.h"
        *tiles++ = 0xD45F;
#include "sun_clock.h"
        for (i = 0; i < count; i++)
#include "sun_clock.h"
            *tiles++ = 0xD05D;
#include "sun_clock.h"
        for (; i <= 9; i++)
#include "sun_clock.h"
            *tiles++ = 0xD05E;
#include "sun_clock.h"
        *tiles = 0xD05F;
#include "sun_clock.h"
    }
#include "sun_clock.h"
    if ((u8)(hud->controlMode - 8) > 1) {
#include "sun_clock.h"
        if (TimedEventValue(hud->actionOwner->timedEventSlot) > 0) {
#include "sun_clock.h"
            tiles = Hud_GetTilemapCell(0, 15, 17);
#include "sun_clock.h"
            for (i = 0; i < 5; i++)
#include "sun_clock.h"
                *tiles++ = (i + 0x3B) | 0xD000u;
#include "sun_clock.h"
            hud->timedEventLabelVisible = 1;
#include "sun_clock.h"
        } else if (hud->timedEventLabelVisible) {
#include "sun_clock.h"
            tiles = Hud_GetTilemapCell(0, 15, 17);
#include "sun_clock.h"
            for (i = 0; i < 5; i++)
#include "sun_clock.h"
                *tiles++ = 0xD000;
#include "sun_clock.h"
            hud->timedEventLabelVisible = 0;
#include "sun_clock.h"
        }
#include "sun_clock.h"
    }
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
extern u16 gUnk_0300517A;
#include "sun_clock.h"

#include "sun_clock.h"
extern u32 gUnk_02000560;
#include "sun_clock.h"

#include "sun_clock.h"
s32 Selection_GetDisplayFrame(s32);
#include "sun_clock.h"

#include "sun_clock.h"
s32 Player_CalculateTotalAttackPower(struct Player *, s32);
#include "sun_clock.h"

#include "sun_clock.h"
s32 Player_GetAttackPower(struct Player *);
#include "sun_clock.h"

#include "sun_clock.h"
enum { GAUGE_TILE_PALETTE = 13 << 12 };
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawTwoDigitCounter(s32 value)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u16 *tiles, *ones;
#include "sun_clock.h"
    s32 tens, remaining;
#include "sun_clock.h"
    if (!(gEngineFlags & 0x1000)) {
#include "sun_clock.h"
        tiles = Hud_GetTilemapCell(0, 25, 0);
#include "sun_clock.h"
        *tiles++ = 0xD0C9;
#include "sun_clock.h"
        *tiles++ = 0xD0CA;
#include "sun_clock.h"
        *tiles++ = 0xD0CB;
#include "sun_clock.h"
        remaining = value;
#include "sun_clock.h"
        if (remaining > 99)
#include "sun_clock.h"
            remaining = 99;
#include "sun_clock.h"
        if (remaining <= 0) {
#include "sun_clock.h"
            tiles[0] = 0xD0CC;
#include "sun_clock.h"
            tiles[1] = 0xD0CC;
#include "sun_clock.h"
        } else {
#include "sun_clock.h"
            tens = 0;
#include "sun_clock.h"
            ones = tiles + 1;
#include "sun_clock.h"
            while (remaining > 9) {
#include "sun_clock.h"
                tens++;
#include "sun_clock.h"
                remaining -= 10;
#include "sun_clock.h"
            }
#include "sun_clock.h"
            tiles[0] = (tens + 0x11) | -12288;
#include "sun_clock.h"
            *ones = (remaining + 0x11) | -12288;
#include "sun_clock.h"
        }
#include "sun_clock.h"
    }
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_ClearCounterTiles(void)
#include "sun_clock.h"
{
#include "sun_clock.h"
    if (!(gEngineFlags & 0x1000)) {
#include "sun_clock.h"
        u16 *tiles = Hud_GetTilemapCell(0, 25, 0);
#include "sun_clock.h"
        s32 i;
#include "sun_clock.h"
        for (i = 0; i < 5; i++)
#include "sun_clock.h"
            *tiles++ = 0xD000;
#include "sun_clock.h"
    }
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawStoredEnergyGauge(struct SceneHudState *hud, s32 mode)
#include "sun_clock.h"
{
#include "sun_clock.h"
    s32 background, foreground, full, partial, n, amount;
#include "sun_clock.h"
    u16 *cell;
#include "sun_clock.h"
    int paletteChanged;
#include "sun_clock.h"
    s32 stored;
#include "sun_clock.h"
    struct GameState *g;
#include "sun_clock.h"
    if (mode == 1) {
#include "sun_clock.h"
        background = 0x1f;
#include "sun_clock.h"
        if (hud->storedEnergy < gGameState->unk618) {
#include "sun_clock.h"
            u16 *timer = &hud->storedEnergyTimer;
#include "sun_clock.h"
            s32 bit;
#include "sun_clock.h"
            ++*timer;
#include "sun_clock.h"
            bit = (*timer >> 2) & 1;
#include "sun_clock.h"
            foreground = 0x7fff;
#include "sun_clock.h"
            if (bit)
#include "sun_clock.h"
                foreground = 0x3e0;
#include "sun_clock.h"
        } else if (hud->storedEnergyFlash) {
#include "sun_clock.h"
            foreground = 0x7fff;
#include "sun_clock.h"
            --hud->storedEnergyFlash;
#include "sun_clock.h"
        } else
#include "sun_clock.h"
            foreground = 0x3e0;
#include "sun_clock.h"
    } else {
#include "sun_clock.h"
        background = 0x39ce;
#include "sun_clock.h"
        if (hud->storedEnergyFlash) {
#include "sun_clock.h"
            amount = 0x7fff;
#include "sun_clock.h"
            foreground = amount;
#include "sun_clock.h"
            --hud->storedEnergyFlash;
#include "sun_clock.h"
        } else
#include "sun_clock.h"
            foreground = 0x1f;
#include "sun_clock.h"
    }
#include "sun_clock.h"
    paletteChanged = gBgPaletteBank13Color14 != background;
#include "sun_clock.h"
    if (paletteChanged)
#include "sun_clock.h"
        gBgPaletteBank13Color14 = background;
#include "sun_clock.h"
    if ((&gBgPaletteBank13Color14)[-1] != foreground)
#include "sun_clock.h"
        (&gBgPaletteBank13Color14)[-1] = foreground;
#include "sun_clock.h"
    g = gGameState;
#include "sun_clock.h"
    stored = g->unk618;
#include "sun_clock.h"
    hud->storedEnergy = stored;
#include "sun_clock.h"
    amount = g->unk618;
#include "sun_clock.h"
    if (amount <= 0) {
#include "sun_clock.h"
        full = 0;
#include "sun_clock.h"
        partial = 0;
#include "sun_clock.h"
    } else if (g->unk61C > 1) {
#include "sun_clock.h"
        s32 scale = 79;
#include "sun_clock.h"
        partial = Div((amount - 1) * scale, g->unk61C - 1) + 1;
#include "sun_clock.h"
        full = partial >> 3;
#include "sun_clock.h"
        partial -= full << 3;
#include "sun_clock.h"
    } else {
#include "sun_clock.h"
        full = 10;
#include "sun_clock.h"
        partial = 0;
#include "sun_clock.h"
    }
#include "sun_clock.h"
    cell = Hud_GetTilemapCell(0, 29, 1);
#include "sun_clock.h"
    *cell-- = 0xd08c;
#include "sun_clock.h"
    *cell-- = 0xd08b;
#include "sun_clock.h"
    *cell-- = 0xd08a;
#include "sun_clock.h"
    for (n = 0; n < full; n++)
#include "sun_clock.h"
        *cell-- = 0xd089;
#include "sun_clock.h"
    if (partial) {
#include "sun_clock.h"
        *cell-- = (partial + 0x81) | -0x3000;
#include "sun_clock.h"
        n++;
#include "sun_clock.h"
    }
#include "sun_clock.h"
    for (; n < 10; n++)
#include "sun_clock.h"
        *cell-- = 0xd04b;
#include "sun_clock.h"
    *cell = 0xd44c;
#include "sun_clock.h"
    Hud_DrawTwoDigitCounter(-1);
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_UpdateTransientStatGauge(struct SceneHudState *hud)
#include "sun_clock.h"
{
#include "sun_clock.h"
    s32 draw = 0, full = 0, partial = 0, width = 0, foreground = 0, background = 0, tile = 0, n;
#include "sun_clock.h"
    u16 *cell;
#include "sun_clock.h"
    if (hud->timedStateTimer) {
#include "sun_clock.h"
        background = 0x39ce;
#include "sun_clock.h"
        if (hud->timedStatePhase) {
#include "sun_clock.h"
            foreground = 0x7fff;
#include "sun_clock.h"
            hud->timedStatePhase--;
#include "sun_clock.h"
        } else
#include "sun_clock.h"
            foreground = 0x1f;
#include "sun_clock.h"
        width = hud->timedStateKind;
#include "sun_clock.h"
        partial = width * 8;
#include "sun_clock.h"
        if (hud->timedStateStart == 0) {
#include "sun_clock.h"
            full = 0;
#include "sun_clock.h"
            partial = 0;
#include "sun_clock.h"
        } else {
#include "sun_clock.h"
            s32 scale = partial - 1;
#include "sun_clock.h"
            partial = Div((hud->timedStateStart - 1) * scale, hud->timedStateValue - 1) + 1;
#include "sun_clock.h"
            full = partial >> 3;
#include "sun_clock.h"
            partial -= full << 3;
#include "sun_clock.h"
        }
#include "sun_clock.h"
        tile = 0x8c;
#include "sun_clock.h"
        --hud->timedStateTimer;
#include "sun_clock.h"
        hud->transientGaugeVisible = 1;
#include "sun_clock.h"
        draw = 1;
#include "sun_clock.h"
        if (hud->secondaryGaugeTimer)
#include "sun_clock.h"
            --hud->secondaryGaugeTimer;
#include "sun_clock.h"
    } else if (hud->secondaryGaugeTimer) {
#include "sun_clock.h"
        background = 0x1f;
#include "sun_clock.h"
        if (hud->secondaryGaugeFlashTimer) {
#include "sun_clock.h"
            foreground = 0x7fff;
#include "sun_clock.h"
            hud->secondaryGaugeFlashTimer--;
#include "sun_clock.h"
        } else
#include "sun_clock.h"
            foreground = 0x3e0;
#include "sun_clock.h"
        width = 10;
#include "sun_clock.h"
        if (hud->secondaryGaugeValue == 0) {
#include "sun_clock.h"
            full = 0;
#include "sun_clock.h"
            partial = 0;
#include "sun_clock.h"
        } else {
#include "sun_clock.h"
            s32 scale = 79;
#include "sun_clock.h"
            partial =
#include "sun_clock.h"
                Div((hud->secondaryGaugeValue - 1) * scale, hud->secondaryGaugeMaximum - 1) + 1;
#include "sun_clock.h"
            full = partial >> 3;
#include "sun_clock.h"
            partial -= full << 3;
#include "sun_clock.h"
        }
#include "sun_clock.h"
        tile = 0x42;
#include "sun_clock.h"
        hud->transientGaugeVisible = 1;
#include "sun_clock.h"
        --hud->secondaryGaugeTimer;
#include "sun_clock.h"
        draw = 1;
#include "sun_clock.h"
    }
#include "sun_clock.h"
    if (draw) {
#include "sun_clock.h"
        if (gBgPaletteBank13Color14 != background)
#include "sun_clock.h"
            gBgPaletteBank13Color14 = background;
#include "sun_clock.h"
        if ((&gBgPaletteBank13Color14)[-1] != foreground)
#include "sun_clock.h"
            (&gBgPaletteBank13Color14)[-1] = foreground;
#include "sun_clock.h"
        cell = Hud_GetTilemapCell(0, 29, 1);
#include "sun_clock.h"
        for (n = 0; n < 3; n++) {
#include "sun_clock.h"
            *cell = tile | (s16)GAUGE_TILE_PALETTE;
#include "sun_clock.h"
            cell--;
#include "sun_clock.h"
            tile--;
#include "sun_clock.h"
        }
#include "sun_clock.h"
        for (n = 0; n < full; n++)
#include "sun_clock.h"
            *cell-- = 0xd089;
#include "sun_clock.h"
        if (partial) {
#include "sun_clock.h"
            *cell-- = (partial + 0x81) | -0x3000;
#include "sun_clock.h"
            n++;
#include "sun_clock.h"
        }
#include "sun_clock.h"
        for (; n < width; n++)
#include "sun_clock.h"
            *cell-- = 0xd04b;
#include "sun_clock.h"
        *cell-- = 0xd44c;
#include "sun_clock.h"
        n++;
#include "sun_clock.h"
        for (; n <= 10; n++)
#include "sun_clock.h"
            *cell-- = 0xd000;
#include "sun_clock.h"
        Hud_DrawTwoDigitCounter((s16)hud->timedStateParameter);
#include "sun_clock.h"
    } else if (hud->transientGaugeVisible) {
#include "sun_clock.h"
        cell = Hud_GetTilemapCell(0, 29, 1);
#include "sun_clock.h"
        for (n = 0; n < 14; n++) {
#include "sun_clock.h"
            *cell = GAUGE_TILE_PALETTE;
#include "sun_clock.h"
            cell--;
#include "sun_clock.h"
        }
#include "sun_clock.h"
        Hud_ClearCounterTiles();
#include "sun_clock.h"
        hud->transientGaugeVisible = 0;
#include "sun_clock.h"
    }
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_UpdateTimedStatusIcon(struct SceneHudState *hud)
#include "sun_clock.h"
{
#include "sun_clock.h"
    /* The shared animation flag write may invalidate the stored owner word. */
#include "sun_clock.h"
    u32 *ownerWord = (u32 *)&hud->actionOwner;
#include "sun_clock.h"
    u16 *cell;
#include "sun_clock.h"
    s32 value, digit;
#include "sun_clock.h"
    struct SceneHudControlAnimation *animation;
#include "sun_clock.h"
    void *resource;
#include "sun_clock.h"
    if (((struct Player *)*ownerWord)->active) {
#include "sun_clock.h"
        hud->controlAnimation.flags &= ~1;
#include "sun_clock.h"
        animation = &hud->controlAnimation;
#include "sun_clock.h"
        resource = hud->motionResource;
#include "sun_clock.h"
        Anim_LoadKeyframe(animation, resource,
#include "sun_clock.h"
                          (u16)(Selection_GetDisplayFrame(((struct Player *)*ownerWord)->unk536)),
#include "sun_clock.h"
                          1);
#include "sun_clock.h"
        cell = Hud_GetTilemapCell(0, 1, 6);
#include "sun_clock.h"
        value = Div(((struct Player *)*ownerWord)->active + 59, 60);
#include "sun_clock.h"
        digit = Div(value, 10);
#include "sun_clock.h"
        value -= digit * 10;
#include "sun_clock.h"
        *cell++ = Hud_GetStyleTileIndex(digit, 0) | -0x3000;
#include "sun_clock.h"
        *cell = Hud_GetStyleTileIndex(value, 0) | -0x3000;
#include "sun_clock.h"
        hud->timedStatusIconVisible = 1;
#include "sun_clock.h"
    } else if (*(s16 *)&gGameState->unk610) {
#include "sun_clock.h"
        hud->controlAnimation.flags &= ~1;
#include "sun_clock.h"
        animation = &hud->controlAnimation;
#include "sun_clock.h"
        resource = hud->motionResource;
#include "sun_clock.h"
        Anim_LoadKeyframe(animation, resource, (u16)(Selection_GetDisplayFrame(0x1b)), 1);
#include "sun_clock.h"
        cell = Hud_GetTilemapCell(0, 1, 6);
#include "sun_clock.h"
        cell[0] = 0xd000;
#include "sun_clock.h"
        cell[1] = 0xd000;
#include "sun_clock.h"
        hud->timedStatusIconVisible = 1;
#include "sun_clock.h"
    } else if (gGameState->unk612) {
#include "sun_clock.h"
        hud->controlAnimation.flags &= ~1;
#include "sun_clock.h"
        animation = &hud->controlAnimation;
#include "sun_clock.h"
        resource = hud->motionResource;
#include "sun_clock.h"
        Anim_LoadKeyframe(animation, resource, (u16)(Selection_GetDisplayFrame(0x1c)), 1);
#include "sun_clock.h"
        cell = Hud_GetTilemapCell(0, 1, 6);
#include "sun_clock.h"
        cell[0] = 0xd000;
#include "sun_clock.h"
        cell[1] = 0xd000;
#include "sun_clock.h"
        hud->timedStatusIconVisible = 1;
#include "sun_clock.h"
    } else if (hud->timedStatusIconVisible) {
#include "sun_clock.h"
        hud->controlAnimation.flags |= 1;
#include "sun_clock.h"
        cell = Hud_GetTilemapCell(0, 1, 6);
#include "sun_clock.h"
        cell[0] = 0xd000;
#include "sun_clock.h"
        cell[1] = 0xd000;
#include "sun_clock.h"
        hud->timedStatusIconVisible = 0;
#include "sun_clock.h"
    }
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_PlayActionChangeCue(struct SceneHudState *hud, u32 group, u32 mode, u32 selection)
#include "sun_clock.h"
{
#include "sun_clock.h"
    if (gEngineFlags & 0x1000) {
#include "sun_clock.h"
        if (group == 0 && mode == 0 && selection - 1 <= 2 && selection != hud->previousCueSelection)
#include "sun_clock.h"
            Sound_PlaySong(0x21C);
#include "sun_clock.h"
    } else {
#include "sun_clock.h"
        if (hud->actionCueInitialized == 0) {
#include "sun_clock.h"
            if (hud->actionCueSuppressed == 0)
#include "sun_clock.h"
                Sound_PlaySong(0x21C);
#include "sun_clock.h"
            hud->actionCueInitialized = 1;
#include "sun_clock.h"
        } else if (group == 0) {
#include "sun_clock.h"
            if (mode == 0 || (mode == 1 && selection == 0)) {
#include "sun_clock.h"
                if (selection != hud->previousCueSelection)
#include "sun_clock.h"
                    Sound_PlaySong(0x21C);
#include "sun_clock.h"
            }
#include "sun_clock.h"
        }
#include "sun_clock.h"
    }
#include "sun_clock.h"
    hud->previousCueGroup = group;
#include "sun_clock.h"
    hud->previousCueSelection = selection;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
static inline u32 TestFlags(u32 mask, u32 *p)
#include "sun_clock.h"
{
#include "sun_clock.h"
    return mask & *p;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
static inline void SetDigitVisibility(struct SceneHudState *hud, u32 mask, s32 hide)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u32 *flags = &hud->countdownDigits[0].flags;
#include "sun_clock.h"
    s32 i = 2;
#include "sun_clock.h"
    if (hide) {
#include "sun_clock.h"
        do {
#include "sun_clock.h"
            *flags |= mask;
#include "sun_clock.h"
            flags += sizeof(struct SceneInputWidget) / sizeof(*flags);
#include "sun_clock.h"
        } while (--i >= 0);
#include "sun_clock.h"
    } else {
#include "sun_clock.h"
        do {
#include "sun_clock.h"
            *flags &= mask;
#include "sun_clock.h"
            flags += sizeof(struct SceneInputWidget) / sizeof(*flags);
#include "sun_clock.h"
        } while (--i >= 0);
#include "sun_clock.h"
    }
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
static inline void HideCountdownDigit(struct SceneInputWidget *digit)
#include "sun_clock.h"
{
#include "sun_clock.h"
    digit->flags |= 1;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_UpdateCountdownDigits(struct SceneHudState *hud)
#include "sun_clock.h"
{
#include "sun_clock.h"
    s32 value, hundreds, tens, ones, base;
#include "sun_clock.h"
    u8 *resource;
#include "sun_clock.h"
    if (hud->countdownEnabled) {
#include "sun_clock.h"
        if (!hud->actionOwner || TestFlags(4, &gUnk_02000560)) {
#include "sun_clock.h"
            SetDigitVisibility(hud, 1, 1);
#include "sun_clock.h"
            hud->countdownEnabled = 0;
#include "sun_clock.h"
        } else {
#include "sun_clock.h"
            value = *hud->countdownValue;
#include "sun_clock.h"
            if (value > 59940) {
#include "sun_clock.h"
                SetDigitVisibility(hud, 1, 1);
#include "sun_clock.h"
                hud->actionCueInitialized = 0;
#include "sun_clock.h"
            } else {
#include "sun_clock.h"
                resource = hud->widgetResource;
#include "sun_clock.h"
                SetDigitVisibility(hud, ~1, 0);
#include "sun_clock.h"
                if (value > 0) {
#include "sun_clock.h"
                    value = Div(value - 1, 60) + 1;
#include "sun_clock.h"
                    hundreds = Div(value, 100);
#include "sun_clock.h"
                    base = hundreds * 100;
#include "sun_clock.h"
                    tens = Div(value - base, 10);
#include "sun_clock.h"
                    ones = value - tens * 10 - base;
#include "sun_clock.h"
                } else {
#include "sun_clock.h"
                    hundreds = 0;
#include "sun_clock.h"
                    tens = 0;
#include "sun_clock.h"
                    ones = 0;
#include "sun_clock.h"
                }
#include "sun_clock.h"
                if ((gEngineFlags & 0x1000) || (*&gGameState->flags868 & 0x400)) {
#include "sun_clock.h"
                    HideCountdownDigit(&hud->countdownDigits[2]);
#include "sun_clock.h"
                    if (hundreds == 0 && tens == 0) {
#include "sun_clock.h"
                        Anim_LoadKeyframe(&hud->countdownDigits[0], resource, (u16)(0x33), 1);
#include "sun_clock.h"
                        Anim_LoadKeyframe(&hud->countdownDigits[1], resource, (u16)(ones + 0x33),
#include "sun_clock.h"
                                          1);
#include "sun_clock.h"
                    } else {
#include "sun_clock.h"
                        Anim_LoadKeyframe(&hud->countdownDigits[0], resource, (u16)(tens + 0x29),
#include "sun_clock.h"
                                          1);
#include "sun_clock.h"
                        Anim_LoadKeyframe(&hud->countdownDigits[1], resource, (u16)(ones + 0x29),
#include "sun_clock.h"
                                          1);
#include "sun_clock.h"
                    }
#include "sun_clock.h"
                } else {
#include "sun_clock.h"
                    if (hundreds == 0 && tens == 0) {
#include "sun_clock.h"
                        Anim_LoadKeyframe(&hud->countdownDigits[0], resource, (u16)(0x33), 1);
#include "sun_clock.h"
                        Anim_LoadKeyframe(&hud->countdownDigits[1], resource, (u16)(0x33), 1);
#include "sun_clock.h"
                        Anim_LoadKeyframe(&hud->countdownDigits[2], resource, (u16)(ones + 0x33),
#include "sun_clock.h"
                                          1);
#include "sun_clock.h"
                    } else {
#include "sun_clock.h"
                        Anim_LoadKeyframe(&hud->countdownDigits[0], resource,
#include "sun_clock.h"
                                          (u16)(hundreds + 0x29), 1);
#include "sun_clock.h"
                        Anim_LoadKeyframe(&hud->countdownDigits[1], resource, (u16)(tens + 0x29),
#include "sun_clock.h"
                                          1);
#include "sun_clock.h"
                        Anim_LoadKeyframe(&hud->countdownDigits[2], resource, (u16)(ones + 0x29),
#include "sun_clock.h"
                                          1);
#include "sun_clock.h"
                    }
#include "sun_clock.h"
                }
#include "sun_clock.h"
                Hud_PlayActionChangeCue(hud, hundreds, tens, ones);
#include "sun_clock.h"
            }
#include "sun_clock.h"
        }
#include "sun_clock.h"
    }
#include "sun_clock.h"
    if (hud->actionCueSuppressed)
#include "sun_clock.h"
        hud->actionCueSuppressed--;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawDecimalDigits(s32 value, s32 divisor, s32 x, s32 y, s32 alternateStyle)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u32 base = 0x31;
#include "sun_clock.h"
    u16 *cell;
#include "sun_clock.h"
    if (!alternateStyle) {
#include "sun_clock.h"
        base = 0x21;
#include "sun_clock.h"
    }
#include "sun_clock.h"
    cell = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
    while (divisor > 0) {
#include "sun_clock.h"
        s32 digit = Div(value, divisor);
#include "sun_clock.h"
        u16 tile = digit + base;
#include "sun_clock.h"
        *cell++ = tile | 0xF000;
#include "sun_clock.h"
        do {
#include "sun_clock.h"
            value -= divisor * digit;
#include "sun_clock.h"
        } while (0);
#include "sun_clock.h"
        divisor = Div(divisor, 10);
#include "sun_clock.h"
    }
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_FillDigitCells(s32 count, u32 x, u32 y, s32 sign)
#include "sun_clock.h"
{
#include "sun_clock.h"
    s32 tile = 1;
#include "sun_clock.h"
    u16 *cell;
#include "sun_clock.h"
    s32 i;
#include "sun_clock.h"
    if (sign >= 0) {
#include "sun_clock.h"
        tile = 0x30;
#include "sun_clock.h"
        if (sign == 0)
#include "sun_clock.h"
            tile = 0x20;
#include "sun_clock.h"
    }
#include "sun_clock.h"
    cell = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
    for (i = 0; i < count; i++)
#include "sun_clock.h"
        *cell++ = (u16)(tile | 0xF000);
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawActiveStatValue(s32 x1, s32 y1, s32 x2, s32 y2, s32 style, s32 mode)
#include "sun_clock.h"
{
#include "sun_clock.h"
    if ((s16)gGameState->unk50 <= 7) {
#include "sun_clock.h"
        Hud_DrawDecimalDigits(Player_CalculateTotalAttackPower(gPlayers[0], mode), 100, x1, y1,
#include "sun_clock.h"
                              style);
#include "sun_clock.h"
        Hud_FillDigitCells(3, x2, y2, style);
#include "sun_clock.h"
    } else {
#include "sun_clock.h"
        Hud_FillDigitCells(3, x1, y1, style);
#include "sun_clock.h"
        Hud_DrawDecimalDigits(Player_GetAttackPower(gPlayers[0]), 100, x2, y2, style);
#include "sun_clock.h"
    }
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawDefaultResourceCounts(s32 x1, s32 y1, s32 x2, s32 y2, s32 style)
#include "sun_clock.h"
{
#include "sun_clock.h"
    Hud_DrawActiveStatValue(x1, y1, x2, y2, style, 0);
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_FillTileRun(s32 count, u32 x, u32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u16 *cell = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
    s32 i;
#include "sun_clock.h"
    for (i = 0; i < count; i++)
#include "sun_clock.h"
        *cell++ = 0xF090;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawCounter7B0(s32 x, s32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u16 *cell = Hud_GetTilemapCell(0, x - 1, y);
#include "sun_clock.h"
    *cell = 0xF001;
#include "sun_clock.h"
    if (gGameState->unk7B0 > 9999)
#include "sun_clock.h"
        gGameState->unk7B0 = 9999;
#include "sun_clock.h"
    Hud_DrawDecimalDigits(gGameState->unk7B0, 1000, x, y, 0);
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawCounter50C(s32 x, s32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u16 *cell = Hud_GetTilemapCell(0, x - 1, y);
#include "sun_clock.h"
    *cell = 0xF001;
#include "sun_clock.h"
    if (gGameState->menuRewardBalance > 9999)
#include "sun_clock.h"
        gGameState->menuRewardBalance = 9999;
#include "sun_clock.h"
    Hud_DrawDecimalDigits(gGameState->menuRewardBalance, 1000, x, y, 1);
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawHeaderCounter(void)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u16 *cell = Hud_GetTilemapCell(0, 0x18, 1);
#include "sun_clock.h"
    *cell = 0xF001;
#include "sun_clock.h"
    if (gGameState->unk77C > 9999)
#include "sun_clock.h"
        gGameState->unk77C = 9999;
#include "sun_clock.h"
    Hud_DrawDecimalDigits(gGameState->unk77C, 1000, 0x19, 1, 0);
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawSelectedCounter(struct SceneHudState *hud)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u16 *cell = Hud_GetTilemapCell(0, 0x12, 1);
#include "sun_clock.h"
    u8 mode = hud->clockSelection;
#include "sun_clock.h"

#include "sun_clock.h"
    if (mode == 0) {
#include "sun_clock.h"
        *cell++ = 0xF023;
#include "sun_clock.h"
        *cell++ = 0xF022;
#include "sun_clock.h"
        *cell++ = 0xF024;
#include "sun_clock.h"
        cell[0] = 0xF09B;
#include "sun_clock.h"
        cell[1] = 0xF001;
#include "sun_clock.h"
        if (gGameState->unk7B0 > 9999)
#include "sun_clock.h"
            gGameState->unk7B0 = 9999;
#include "sun_clock.h"
        Hud_DrawCounter7B0(0x17, 1);
#include "sun_clock.h"
    } else if (mode == 1) {
#include "sun_clock.h"
        *cell++ = 0xF025;
#include "sun_clock.h"
        *cell++ = 0xF026;
#include "sun_clock.h"
        *cell++ = 0xF022;
#include "sun_clock.h"
        cell[0] = 0xF09B;
#include "sun_clock.h"
        cell[1] = 0xF030;
#include "sun_clock.h"
        if (gGameState->menuRewardBalance > 9999)
#include "sun_clock.h"
            gGameState->menuRewardBalance = 9999;
#include "sun_clock.h"
        Hud_DrawCounter50C(0x17, 1);
#include "sun_clock.h"
    } else {
#include "sun_clock.h"
        *cell++ = 0xF0B9;
#include "sun_clock.h"
        *cell++ = 0xF0BA;
#include "sun_clock.h"
        *cell++ = 0xF022;
#include "sun_clock.h"
        cell[0] = 0xF0BB;
#include "sun_clock.h"
        cell[1] = 0xF09B;
#include "sun_clock.h"
        if (gGameState->unk77C > 9999)
#include "sun_clock.h"
            gGameState->unk77C = 9999;
#include "sun_clock.h"
    }
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawSunClockTimeB(s32 x, s32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    Hud_DrawDecimalDigits(gSunClock.hourB, 10, x, y, 0);
#include "sun_clock.h"
    Hud_DrawDecimalDigits(gSunClock.minuteB, 10, x + 3, y, 0);
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawSunClockTimeA(s32 x, s32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    Hud_DrawDecimalDigits(gSunClock.hourA, 10, x, y, 0);
#include "sun_clock.h"
    Hud_DrawDecimalDigits(gSunClock.minuteA, 10, x + 3, y, 0);
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawClockLabelB(u32 x, u32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u16 *cell = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
    *cell = 0xF07C;
#include "sun_clock.h"
    cell++;
#include "sun_clock.h"
    *cell = 0xF07D;
#include "sun_clock.h"
    cell[1] = 0xF07F;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawClockLabelA(u32 x, u32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    u16 *cell = Hud_GetTilemapCell(0, x, y);
#include "sun_clock.h"
    *cell = 0xF07C;
#include "sun_clock.h"
    cell++;
#include "sun_clock.h"
    *cell = 0xF07D;
#include "sun_clock.h"
    cell[1] = 0xF07E;
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawSelectedClock(struct SceneHudState *hud)
#include "sun_clock.h"
{
#include "sun_clock.h"
    if ((u8)(hud->mode - 1) <= 1 || hud->mode == 5 || hud->mode == 3 || hud->mode == 4 ||
#include "sun_clock.h"
        hud->mode == 6) {
#include "sun_clock.h"
        if (hud->clockSelection == 0) {
#include "sun_clock.h"
            Hud_DrawDecimalDigits(SunClock_GetBaseHour(), 10, 24, 0, 0);
#include "sun_clock.h"
            Hud_DrawDecimalDigits(SunClock_GetBaseMinute(), 10, 27, 0, 0);
#include "sun_clock.h"
        } else if (hud->clockSelection == 1) {
#include "sun_clock.h"
            Hud_DrawSunClockTimeA(24, 0);
#include "sun_clock.h"
        } else {
#include "sun_clock.h"
            Hud_DrawSunClockTimeB(24, 0);
#include "sun_clock.h"
        }
#include "sun_clock.h"
    }
#include "sun_clock.h"
}
#include "sun_clock.h"

#include "sun_clock.h"
void Hud_DrawStatusMetersAndCounts(struct SceneHudState *hud, s32 x, s32 y)
#include "sun_clock.h"
{
#include "sun_clock.h"
    Hud_DrawDefaultHealthMeter(hud, 1, x, y);
#include "sun_clock.h"
    y += 2;
#include "sun_clock.h"
    Hud_DrawDefaultEnergyMeter(hud, 1, x, y, hud->actionOwner->currentStatB,
#include "sun_clock.h"
                               hud->actionOwner->maximumStatB);
#include "sun_clock.h"
    Hud_DrawDefaultResourceCounts(8, 4, 8, 3, 0);
#include "sun_clock.h"
}
#include "sun_clock.h"
