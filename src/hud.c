#include "hud.h"
#include "link_session_state.h"

extern u16 gUnk_03005178;

void Hud_DrawHealthMeterCells(struct SceneHudState *hud, s32 mode, s32 x, s32 y, s32 cap, s32 value,
                              s32 maximum)
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
        units = Div((value - 1) * 79, maximum - 1) + 1;
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
            u16 *timer = &hud->healthMeterBlinkTimer;
            u16 t = *timer;
            s32 bit = (t >> 2) & 1;
            color = 0x12;
            if (bit)
                color = 0x109f;
            *timer = t + 1;
        }
        if (cap == 0)
            cell = Hud_GetTilemapCell(0, x, y);
        else
            cell = Hud_GetTilemapCell(0, x - 1, y);
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
            cell = Hud_GetTilemapCell(0, x - 2, y);
        else
            cell = Hud_GetTilemapCell(0, x - 1, y);
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

void Hud_DrawHealthMeter(struct SceneHudState *hud, u32 mode, u32 x, u32 y, u32 cap)
{
    Hud_DrawHealthMeterCells(hud, mode, x, y, cap, hud->actionOwner->currentStatA,
                             hud->actionOwner->maximumStatA);
}

void Hud_DrawDefaultHealthMeter(struct SceneHudState *hud, u32 mode, u32 x, u32 y)
{
    Hud_DrawHealthMeter(hud, mode, x, y, 0);
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

void Hud_DrawEnergyMeter(struct SceneHudState *hud, s32 mode, s32 x, s32 y, s32 value, s32 maximum,
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
        units = Div((value - 1) * 79, maximum - 1) + 1;
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
                c = Hud_GetTilemapCell(0, x, y);
            else
                c = Hud_GetTilemapCell(0, x - 1, y);
        } else {
            fullTile = 0x73;
            emptyTile = 0x6b;
            endTile = 0x6c;
            if (cap != 0)
                c = Hud_GetTilemapCell(0, x - 2, y);
            else
                c = Hud_GetTilemapCell(0, x - 1, y);
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

void Hud_DrawDefaultEnergyMeter(struct SceneHudState *hud, s32 mode, s32 x, s32 y, s32 value,
                                s32 maximum)
{
    Hud_DrawEnergyMeter(hud, mode, x, y, value, maximum, 0);
}

void Hud_DrawHealthLabel(s32 unused, s32 x, s32 y)
{
    u16 *cell = Hud_GetTilemapCell(0, x, y);
    *cell++ = 0xD040;
    *cell++ = 0xD041;
    *cell = 0xD042;
}

void Hud_DrawEnergyLabel(s32 unused, s32 x, s32 y)
{
    u16 *cell = Hud_GetTilemapCell(0, x, y);
    *cell++ = 0xD050;
    *cell++ = 0xD051;
    *cell = 0xD052;
}

void Hud_DrawAlternateMeterLabel(s32 unused, s32 x, s32 y)
{
    u16 *cell = Hud_GetTilemapCell(0, x, y);
    *cell++ = 0xD08D;
    *cell++ = 0xD08E;
    *cell = 0xD08F;
}

void Hud_DrawPlayerGauge(struct SceneHudState *hud, s32 x, s32 y)
{
    s32 filled, i;
    u16 *tiles;
    if (!hud->playerValueEnabled) {
        tiles = Hud_GetTilemapCell(0, x - 3, y);
        i = 0;
        do {
            *tiles++ = 0xD000;
        } while (++i < 14);
    } else {
        if ((u8)(hud->actionOwner->actionMode - 1) <= 1) {
            if (hud->actionOwner->statGaugeFull == 1)
                filled = 10;
            else
                filled = Div(hud->actionOwner->statusValue, 400);
        } else
            filled = hud->actionOwner->statWhole;
        tiles = Hud_GetTilemapCell(0, x, y);
        for (i = 0; i < filled; i++)
            *tiles++ = 0xD05C;
        for (; i < 10; i++)
            *tiles++ = 0xD05E;
    }
}

static inline s32 OwnerIndex(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}

static inline u8 SlotCount(s32 slot)
{
    if (!gLinkSession || slot < 0)
        return 0;
    return gLinkSession->playerValues[slot];
}

static inline u32 testFlags(const u32 *flags, u32 mask)
{
    return *flags & mask;
}

static inline u8 testFlag(u32 mask, u32 *flags)
{
    if (*flags & mask)
        return TRUE;
    return FALSE;
}

static inline u32 LinkFlags(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}

static inline s32 TimedEventValue(u32 slot)
{
    return ((struct GameState *)(slot * sizeof(s32) + (u32)gGameState))->counters628[0];
}

void Hud_DrawWarningMeter(struct SceneHudState *hud, s32 mode, u32 x, u32 y)
{
    s32 blink;
    u16 *tiles;
    s32 i;
    u32 pulse;
    s32 count;

    if (hud->actionOwner->tetherMode == 0x23 && hud->controlMode != 8)
        goto blank;
    if (gGameState->playerParticleRate > 0x9C3 && hud->controlMode != 8) {
        u16 *timer = &hud->warningBlinkTimer;
        u32 bit = (*timer >> 4) & 1;
        blink = 1;
        if (bit)
            blink = 0;
        if ((*timer & 0x3F) == 2)
            Sound_PlaySong(0x132);
        (*timer)++;
    } else if (gGameState->playerParticleRate > 0 && hud->controlMode != 8) {
        u16 *timer = &hud->warningBlinkTimer;
        u32 bit = (*timer >> 5) & 1;
        blink = 1;
        if (bit)
            blink = 0;
        if ((*timer & 0x3F) == 2)
            Sound_PlaySong(0x131);
        (*timer)++;
    } else {
        blink = 0;
        hud->warningBlinkTimer = blink;
    }
    if (blink) {
    blank:
        tiles = Hud_GetTilemapCell(0, x, y);
        for (i = 0; i < 12; i++)
            *tiles++ = (i + 3) | 0xD000;
    } else {
        tiles = Hud_GetTilemapCell(0, x, y);
        if (mode == 1)
            pulse = 0x29F;
        else if (testFlag(0x80, &gGameState->flags868) || hud->controlMode == 8 ||
                 testFlags(&hud->actionOwner->statusFlags, 0x10)) {
            u32 frame = hud->meterPulseFrame;
            pulse = 0x29F;
            if (frame > 0x24) {
                pulse = 0x1FB;
                if (frame <= 0x2C)
                    goto pulse_ready;
                if (frame <= 0x34)
                    pulse = 0x177;
            }
        pulse_ready:
            if (++hud->meterPulseFrame > 0x3B)
                hud->meterPulseFrame = 0;
        } else {
            hud->meterPulseFrame = 0x34;
            pulse = 0x177;
        }
        if (gBgPaletteBank13Color10 != pulse)
            gBgPaletteBank13Color10 = pulse;
        if (LinkFlags(0x800))
            count = SlotCount(OwnerIndex());
        else
            count = gGameState->unk876;
        *tiles++ = 0xD45F;
        for (i = 0; i < count; i++)
            *tiles++ = 0xD05D;
        for (; i <= 9; i++)
            *tiles++ = 0xD05E;
        *tiles = 0xD05F;
    }
    if ((u8)(hud->controlMode - 8) > 1) {
        if (TimedEventValue(hud->actionOwner->timedEventSlot) > 0) {
            tiles = Hud_GetTilemapCell(0, 15, 17);
            for (i = 0; i < 5; i++)
                *tiles++ = (i + 0x3B) | 0xD000u;
            hud->timedEventLabelVisible = 1;
        } else if (hud->timedEventLabelVisible) {
            tiles = Hud_GetTilemapCell(0, 15, 17);
            for (i = 0; i < 5; i++)
                *tiles++ = 0xD000;
            hud->timedEventLabelVisible = 0;
        }
    }
}

extern u16 gUnk_0300517A;

extern u32 gUnk_02000560;

s32 Selection_GetDisplayFrame(s32);

s32 Player_CalculateTotalAttackPower(struct Player *, s32);

s32 Player_GetAttackPower(struct Player *);

enum { GAUGE_TILE_PALETTE = 13 << 12 };

void Hud_DrawTwoDigitCounter(s32 value)
{
    u16 *tiles, *ones;
    s32 tens, remaining;
    if (!(gEngineFlags & 0x1000)) {
        tiles = Hud_GetTilemapCell(0, 25, 0);
        *tiles++ = 0xD0C9;
        *tiles++ = 0xD0CA;
        *tiles++ = 0xD0CB;
        remaining = value;
        if (remaining > 99)
            remaining = 99;
        if (remaining <= 0) {
            tiles[0] = 0xD0CC;
            tiles[1] = 0xD0CC;
        } else {
            tens = 0;
            ones = tiles + 1;
            while (remaining > 9) {
                tens++;
                remaining -= 10;
            }
            tiles[0] = (tens + 0x11) | -12288;
            *ones = (remaining + 0x11) | -12288;
        }
    }
}

void Hud_ClearCounterTiles(void)
{
    if (!(gEngineFlags & 0x1000)) {
        u16 *tiles = Hud_GetTilemapCell(0, 25, 0);
        s32 i;
        for (i = 0; i < 5; i++)
            *tiles++ = 0xD000;
    }
}

void Hud_DrawStoredEnergyGauge(struct SceneHudState *hud, s32 mode)
{
    s32 background, foreground, full, partial, n, amount;
    u16 *cell;
    int paletteChanged;
    s32 stored;
    struct GameState *g;
    if (mode == 1) {
        background = 0x1f;
        if (hud->storedEnergy < gGameState->unk618) {
            u16 *timer = &hud->storedEnergyTimer;
            s32 bit;
            ++*timer;
            bit = (*timer >> 2) & 1;
            foreground = 0x7fff;
            if (bit)
                foreground = 0x3e0;
        } else if (hud->storedEnergyFlash) {
            foreground = 0x7fff;
            --hud->storedEnergyFlash;
        } else
            foreground = 0x3e0;
    } else {
        background = 0x39ce;
        if (hud->storedEnergyFlash) {
            amount = 0x7fff;
            foreground = amount;
            --hud->storedEnergyFlash;
        } else
            foreground = 0x1f;
    }
    paletteChanged = gBgPaletteBank13Color14 != background;
    if (paletteChanged)
        gBgPaletteBank13Color14 = background;
    if ((&gBgPaletteBank13Color14)[-1] != foreground)
        (&gBgPaletteBank13Color14)[-1] = foreground;
    g = gGameState;
    stored = g->unk618;
    hud->storedEnergy = stored;
    amount = g->unk618;
    if (amount <= 0) {
        full = 0;
        partial = 0;
    } else if (g->unk61C > 1) {
        s32 scale = 79;
        partial = Div((amount - 1) * scale, g->unk61C - 1) + 1;
        full = partial >> 3;
        partial -= full << 3;
    } else {
        full = 10;
        partial = 0;
    }
    cell = Hud_GetTilemapCell(0, 29, 1);
    *cell-- = 0xd08c;
    *cell-- = 0xd08b;
    *cell-- = 0xd08a;
    for (n = 0; n < full; n++)
        *cell-- = 0xd089;
    if (partial) {
        *cell-- = (partial + 0x81) | -0x3000;
        n++;
    }
    for (; n < 10; n++)
        *cell-- = 0xd04b;
    *cell = 0xd44c;
    Hud_DrawTwoDigitCounter(-1);
}

void Hud_UpdateTransientStatGauge(struct SceneHudState *hud)
{
    s32 draw = 0, full = 0, partial = 0, width = 0, foreground = 0, background = 0, tile = 0, n;
    u16 *cell;
    if (hud->timedStateTimer) {
        background = 0x39ce;
        if (hud->timedStatePhase) {
            foreground = 0x7fff;
            hud->timedStatePhase--;
        } else
            foreground = 0x1f;
        width = hud->timedStateKind;
        partial = width * 8;
        if (hud->timedStateStart == 0) {
            full = 0;
            partial = 0;
        } else {
            s32 scale = partial - 1;
            partial = Div((hud->timedStateStart - 1) * scale, hud->timedStateValue - 1) + 1;
            full = partial >> 3;
            partial -= full << 3;
        }
        tile = 0x8c;
        --hud->timedStateTimer;
        hud->transientGaugeVisible = 1;
        draw = 1;
        if (hud->secondaryGaugeTimer)
            --hud->secondaryGaugeTimer;
    } else if (hud->secondaryGaugeTimer) {
        background = 0x1f;
        if (hud->secondaryGaugeFlashTimer) {
            foreground = 0x7fff;
            hud->secondaryGaugeFlashTimer--;
        } else
            foreground = 0x3e0;
        width = 10;
        if (hud->secondaryGaugeValue == 0) {
            full = 0;
            partial = 0;
        } else {
            s32 scale = 79;
            partial =
                Div((hud->secondaryGaugeValue - 1) * scale, hud->secondaryGaugeMaximum - 1) + 1;
            full = partial >> 3;
            partial -= full << 3;
        }
        tile = 0x42;
        hud->transientGaugeVisible = 1;
        --hud->secondaryGaugeTimer;
        draw = 1;
    }
    if (draw) {
        if (gBgPaletteBank13Color14 != background)
            gBgPaletteBank13Color14 = background;
        if ((&gBgPaletteBank13Color14)[-1] != foreground)
            (&gBgPaletteBank13Color14)[-1] = foreground;
        cell = Hud_GetTilemapCell(0, 29, 1);
        for (n = 0; n < 3; n++) {
            *cell = tile | (s16)GAUGE_TILE_PALETTE;
            cell--;
            tile--;
        }
        for (n = 0; n < full; n++)
            *cell-- = 0xd089;
        if (partial) {
            *cell-- = (partial + 0x81) | -0x3000;
            n++;
        }
        for (; n < width; n++)
            *cell-- = 0xd04b;
        *cell-- = 0xd44c;
        n++;
        for (; n <= 10; n++)
            *cell-- = 0xd000;
        Hud_DrawTwoDigitCounter((s16)hud->timedStateParameter);
    } else if (hud->transientGaugeVisible) {
        cell = Hud_GetTilemapCell(0, 29, 1);
        for (n = 0; n < 14; n++) {
            *cell = GAUGE_TILE_PALETTE;
            cell--;
        }
        Hud_ClearCounterTiles();
        hud->transientGaugeVisible = 0;
    }
}

void Hud_UpdateTimedStatusIcon(struct SceneHudState *hud)
{
    /* The shared animation flag write may invalidate the stored owner word. */
    u32 *ownerWord = (u32 *)&hud->actionOwner;
    u16 *cell;
    s32 value, digit;
    struct SceneHudControlAnimation *animation;
    void *resource;
    if (((struct Player *)*ownerWord)->active) {
        hud->controlAnimation.flags &= ~1;
        animation = &hud->controlAnimation;
        resource = hud->motionResource;
        Anim_LoadKeyframe(animation, resource,
                          (u16)(Selection_GetDisplayFrame(((struct Player *)*ownerWord)->unk536)),
                          1);
        cell = Hud_GetTilemapCell(0, 1, 6);
        value = Div(((struct Player *)*ownerWord)->active + 59, 60);
        digit = Div(value, 10);
        value -= digit * 10;
        *cell++ = Hud_GetStyleTileIndex(digit, 0) | -0x3000;
        *cell = Hud_GetStyleTileIndex(value, 0) | -0x3000;
        hud->timedStatusIconVisible = 1;
    } else if (*(s16 *)&gGameState->unk610) {
        hud->controlAnimation.flags &= ~1;
        animation = &hud->controlAnimation;
        resource = hud->motionResource;
        Anim_LoadKeyframe(animation, resource, (u16)(Selection_GetDisplayFrame(0x1b)), 1);
        cell = Hud_GetTilemapCell(0, 1, 6);
        cell[0] = 0xd000;
        cell[1] = 0xd000;
        hud->timedStatusIconVisible = 1;
    } else if (gGameState->unk612) {
        hud->controlAnimation.flags &= ~1;
        animation = &hud->controlAnimation;
        resource = hud->motionResource;
        Anim_LoadKeyframe(animation, resource, (u16)(Selection_GetDisplayFrame(0x1c)), 1);
        cell = Hud_GetTilemapCell(0, 1, 6);
        cell[0] = 0xd000;
        cell[1] = 0xd000;
        hud->timedStatusIconVisible = 1;
    } else if (hud->timedStatusIconVisible) {
        hud->controlAnimation.flags |= 1;
        cell = Hud_GetTilemapCell(0, 1, 6);
        cell[0] = 0xd000;
        cell[1] = 0xd000;
        hud->timedStatusIconVisible = 0;
    }
}

void Hud_PlayActionChangeCue(struct SceneHudState *hud, u32 group, u32 mode, u32 selection)
{
    if (gEngineFlags & 0x1000) {
        if (group == 0 && mode == 0 && selection - 1 <= 2 && selection != hud->previousCueSelection)
            Sound_PlaySong(0x21C);
    } else {
        if (hud->actionCueInitialized == 0) {
            if (hud->actionCueSuppressed == 0)
                Sound_PlaySong(0x21C);
            hud->actionCueInitialized = 1;
        } else if (group == 0) {
            if (mode == 0 || (mode == 1 && selection == 0)) {
                if (selection != hud->previousCueSelection)
                    Sound_PlaySong(0x21C);
            }
        }
    }
    hud->previousCueGroup = group;
    hud->previousCueSelection = selection;
}

static inline u32 TestFlags(u32 mask, u32 *p)
{
    return mask & *p;
}

static inline void SetDigitVisibility(struct SceneHudState *hud, u32 mask, s32 hide)
{
    u32 *flags = &hud->countdownDigits[0].flags;
    s32 i = 2;
    if (hide) {
        do {
            *flags |= mask;
            flags += sizeof(struct SceneInputWidget) / sizeof(*flags);
        } while (--i >= 0);
    } else {
        do {
            *flags &= mask;
            flags += sizeof(struct SceneInputWidget) / sizeof(*flags);
        } while (--i >= 0);
    }
}

static inline void HideCountdownDigit(struct SceneInputWidget *digit)
{
    digit->flags |= 1;
}

void Hud_UpdateCountdownDigits(struct SceneHudState *hud)
{
    s32 value, hundreds, tens, ones, base;
    u8 *resource;
    if (hud->countdownEnabled) {
        if (!hud->actionOwner || TestFlags(4, &gUnk_02000560)) {
            SetDigitVisibility(hud, 1, 1);
            hud->countdownEnabled = 0;
        } else {
            value = *hud->countdownValue;
            if (value > 59940) {
                SetDigitVisibility(hud, 1, 1);
                hud->actionCueInitialized = 0;
            } else {
                resource = hud->widgetResource;
                SetDigitVisibility(hud, ~1, 0);
                if (value > 0) {
                    value = Div(value - 1, 60) + 1;
                    hundreds = Div(value, 100);
                    base = hundreds * 100;
                    tens = Div(value - base, 10);
                    ones = value - tens * 10 - base;
                } else {
                    hundreds = 0;
                    tens = 0;
                    ones = 0;
                }
                if ((gEngineFlags & 0x1000) || (*&gGameState->flags868 & 0x400)) {
                    HideCountdownDigit(&hud->countdownDigits[2]);
                    if (hundreds == 0 && tens == 0) {
                        Anim_LoadKeyframe(&hud->countdownDigits[0], resource, (u16)(0x33), 1);
                        Anim_LoadKeyframe(&hud->countdownDigits[1], resource, (u16)(ones + 0x33),
                                          1);
                    } else {
                        Anim_LoadKeyframe(&hud->countdownDigits[0], resource, (u16)(tens + 0x29),
                                          1);
                        Anim_LoadKeyframe(&hud->countdownDigits[1], resource, (u16)(ones + 0x29),
                                          1);
                    }
                } else {
                    if (hundreds == 0 && tens == 0) {
                        Anim_LoadKeyframe(&hud->countdownDigits[0], resource, (u16)(0x33), 1);
                        Anim_LoadKeyframe(&hud->countdownDigits[1], resource, (u16)(0x33), 1);
                        Anim_LoadKeyframe(&hud->countdownDigits[2], resource, (u16)(ones + 0x33),
                                          1);
                    } else {
                        Anim_LoadKeyframe(&hud->countdownDigits[0], resource,
                                          (u16)(hundreds + 0x29), 1);
                        Anim_LoadKeyframe(&hud->countdownDigits[1], resource, (u16)(tens + 0x29),
                                          1);
                        Anim_LoadKeyframe(&hud->countdownDigits[2], resource, (u16)(ones + 0x29),
                                          1);
                    }
                }
                Hud_PlayActionChangeCue(hud, hundreds, tens, ones);
            }
        }
    }
    if (hud->actionCueSuppressed)
        hud->actionCueSuppressed--;
}

void Hud_DrawDecimalDigits(s32 value, s32 divisor, s32 x, s32 y, s32 alternateStyle)
{
    u32 base = 0x31;
    u16 *cell;
    if (!alternateStyle) {
        base = 0x21;
    }
    cell = Hud_GetTilemapCell(0, x, y);
    while (divisor > 0) {
        s32 digit = Div(value, divisor);
        u16 tile = digit + base;
        *cell++ = tile | 0xF000;
        do {
            value -= divisor * digit;
        } while (0);
        divisor = Div(divisor, 10);
    }
}

void Hud_FillDigitCells(s32 count, u32 x, u32 y, s32 sign)
{
    s32 tile = 1;
    u16 *cell;
    s32 i;
    if (sign >= 0) {
        tile = 0x30;
        if (sign == 0)
            tile = 0x20;
    }
    cell = Hud_GetTilemapCell(0, x, y);
    for (i = 0; i < count; i++)
        *cell++ = (u16)(tile | 0xF000);
}

void Hud_DrawActiveStatValue(s32 x1, s32 y1, s32 x2, s32 y2, s32 style, s32 mode)
{
    if ((s16)gGameState->unk50 <= 7) {
        Hud_DrawDecimalDigits(Player_CalculateTotalAttackPower(gPlayers[0], mode), 100, x1, y1,
                              style);
        Hud_FillDigitCells(3, x2, y2, style);
    } else {
        Hud_FillDigitCells(3, x1, y1, style);
        Hud_DrawDecimalDigits(Player_GetAttackPower(gPlayers[0]), 100, x2, y2, style);
    }
}

void Hud_DrawDefaultResourceCounts(s32 x1, s32 y1, s32 x2, s32 y2, s32 style)
{
    Hud_DrawActiveStatValue(x1, y1, x2, y2, style, 0);
}

void Hud_FillTileRun(s32 count, u32 x, u32 y)
{
    u16 *cell = Hud_GetTilemapCell(0, x, y);
    s32 i;
    for (i = 0; i < count; i++)
        *cell++ = 0xF090;
}

void Hud_DrawCounter7B0(s32 x, s32 y)
{
    u16 *cell = Hud_GetTilemapCell(0, x - 1, y);
    *cell = 0xF001;
    if (gGameState->unk7B0 > 9999)
        gGameState->unk7B0 = 9999;
    Hud_DrawDecimalDigits(gGameState->unk7B0, 1000, x, y, 0);
}

void Hud_DrawCounter50C(s32 x, s32 y)
{
    u16 *cell = Hud_GetTilemapCell(0, x - 1, y);
    *cell = 0xF001;
    if (gGameState->menuRewardBalance > 9999)
        gGameState->menuRewardBalance = 9999;
    Hud_DrawDecimalDigits(gGameState->menuRewardBalance, 1000, x, y, 1);
}

void Hud_DrawHeaderCounter(void)
{
    u16 *cell = Hud_GetTilemapCell(0, 0x18, 1);
    *cell = 0xF001;
    if (gGameState->unk77C > 9999)
        gGameState->unk77C = 9999;
    Hud_DrawDecimalDigits(gGameState->unk77C, 1000, 0x19, 1, 0);
}

void Hud_DrawSelectedCounter(struct SceneHudState *hud)
{
    u16 *cell = Hud_GetTilemapCell(0, 0x12, 1);
    u8 mode = hud->clockSelection;

    if (mode == 0) {
        *cell++ = 0xF023;
        *cell++ = 0xF022;
        *cell++ = 0xF024;
        cell[0] = 0xF09B;
        cell[1] = 0xF001;
        if (gGameState->unk7B0 > 9999)
            gGameState->unk7B0 = 9999;
        Hud_DrawCounter7B0(0x17, 1);
    } else if (mode == 1) {
        *cell++ = 0xF025;
        *cell++ = 0xF026;
        *cell++ = 0xF022;
        cell[0] = 0xF09B;
        cell[1] = 0xF030;
        if (gGameState->menuRewardBalance > 9999)
            gGameState->menuRewardBalance = 9999;
        Hud_DrawCounter50C(0x17, 1);
    } else {
        *cell++ = 0xF0B9;
        *cell++ = 0xF0BA;
        *cell++ = 0xF022;
        cell[0] = 0xF0BB;
        cell[1] = 0xF09B;
        if (gGameState->unk77C > 9999)
            gGameState->unk77C = 9999;
    }
}

void Hud_DrawSunClockTimeB(s32 x, s32 y)
{
    Hud_DrawDecimalDigits(gSunClock.hourB, 10, x, y, 0);
    Hud_DrawDecimalDigits(gSunClock.minuteB, 10, x + 3, y, 0);
}

void Hud_DrawSunClockTimeA(s32 x, s32 y)
{
    Hud_DrawDecimalDigits(gSunClock.hourA, 10, x, y, 0);
    Hud_DrawDecimalDigits(gSunClock.minuteA, 10, x + 3, y, 0);
}

void Hud_DrawClockLabelB(u32 x, u32 y)
{
    u16 *cell = Hud_GetTilemapCell(0, x, y);
    *cell = 0xF07C;
    cell++;
    *cell = 0xF07D;
    cell[1] = 0xF07F;
}

void Hud_DrawClockLabelA(u32 x, u32 y)
{
    u16 *cell = Hud_GetTilemapCell(0, x, y);
    *cell = 0xF07C;
    cell++;
    *cell = 0xF07D;
    cell[1] = 0xF07E;
}

void Hud_DrawSelectedClock(struct SceneHudState *hud)
{
    if ((u8)(hud->mode - 1) <= 1 || hud->mode == 5 || hud->mode == 3 || hud->mode == 4 ||
        hud->mode == 6) {
        if (hud->clockSelection == 0) {
            Hud_DrawDecimalDigits(SunClock_GetBaseHour(), 10, 24, 0, 0);
            Hud_DrawDecimalDigits(SunClock_GetBaseMinute(), 10, 27, 0, 0);
        } else if (hud->clockSelection == 1) {
            Hud_DrawSunClockTimeA(24, 0);
        } else {
            Hud_DrawSunClockTimeB(24, 0);
        }
    }
}

void Hud_DrawStatusMetersAndCounts(struct SceneHudState *hud, s32 x, s32 y)
{
    Hud_DrawDefaultHealthMeter(hud, 1, x, y);
    y += 2;
    Hud_DrawDefaultEnergyMeter(hud, 1, x, y, hud->actionOwner->currentStatB,
                               hud->actionOwner->maximumStatB);
    Hud_DrawDefaultResourceCounts(8, 4, 8, 3, 0);
}
