#include "hud.h"
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

void Hud_UpdateTransientStatGauge(struct SceneHudState *p)
{
    s32 draw = 0, full = 0, partial = 0, width = 0, foreground = 0, background = 0, tile = 0, n;
    u16 *cell;
    if (p->timedStateTimer) {
        background = 0x39ce;
        if (p->timedStatePhase) {
            foreground = 0x7fff;
            p->timedStatePhase--;
        } else
            foreground = 0x1f;
        width = p->timedStateKind;
        partial = width * 8;
        if (p->timedStateStart == 0) {
            full = 0;
            partial = 0;
        } else {
            s32 scale = partial - 1;
            partial = Div((p->timedStateStart - 1) * scale, p->timedStateValue - 1) + 1;
            full = partial >> 3;
            partial -= full << 3;
        }
        tile = 0x8c;
        --p->timedStateTimer;
        p->transientGaugeVisible = 1;
        draw = 1;
        if (p->secondaryGaugeTimer)
            --p->secondaryGaugeTimer;
    } else if (p->secondaryGaugeTimer) {
        background = 0x1f;
        if (p->secondaryGaugeFlashTimer) {
            foreground = 0x7fff;
            p->secondaryGaugeFlashTimer--;
        } else
            foreground = 0x3e0;
        width = 10;
        if (p->secondaryGaugeValue == 0) {
            full = 0;
            partial = 0;
        } else {
            s32 scale = 79;
            partial = Div((p->secondaryGaugeValue - 1) * scale, p->secondaryGaugeMaximum - 1) + 1;
            full = partial >> 3;
            partial -= full << 3;
        }
        tile = 0x42;
        p->transientGaugeVisible = 1;
        --p->secondaryGaugeTimer;
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
        Hud_DrawTwoDigitCounter((s16)p->timedStateParameter);
    } else if (p->transientGaugeVisible) {
        cell = Hud_GetTilemapCell(0, 29, 1);
        for (n = 0; n < 14; n++) {
            *cell = GAUGE_TILE_PALETTE;
            cell--;
        }
        Hud_ClearCounterTiles();
        p->transientGaugeVisible = 0;
    }
}

void Hud_UpdateTimedStatusIcon(struct SceneHudState *p)
{
    /* The shared animation flag write may invalidate the stored owner word. */
    u32 *player = (u32 *)&p->actionOwner;
    u16 *cell;
    s32 value, digit;
    struct SceneHudControlAnimation *anim;
    void *res;
    if (((struct Player *)*player)->active) {
        p->controlAnimation.flags &= ~1;
        anim = &p->controlAnimation;
        res = p->motionResource;
        Anim_LoadKeyframe(anim, res,
                          (u16)(Selection_GetDisplayFrame(((struct Player *)*player)->unk536)), 1);
        cell = Hud_GetTilemapCell(0, 1, 6);
        value = Div(((struct Player *)*player)->active + 59, 60);
        digit = Div(value, 10);
        value -= digit * 10;
        *cell++ = Hud_GetStyleTileIndex(digit, 0) | -0x3000;
        *cell = Hud_GetStyleTileIndex(value, 0) | -0x3000;
        p->timedStatusIconVisible = 1;
    } else if (*(s16 *)&gGameState->unk610) {
        p->controlAnimation.flags &= ~1;
        anim = &p->controlAnimation;
        res = p->motionResource;
        Anim_LoadKeyframe(anim, res, (u16)(Selection_GetDisplayFrame(0x1b)), 1);
        cell = Hud_GetTilemapCell(0, 1, 6);
        cell[0] = 0xd000;
        cell[1] = 0xd000;
        p->timedStatusIconVisible = 1;
    } else if (gGameState->unk612) {
        p->controlAnimation.flags &= ~1;
        anim = &p->controlAnimation;
        res = p->motionResource;
        Anim_LoadKeyframe(anim, res, (u16)(Selection_GetDisplayFrame(0x1c)), 1);
        cell = Hud_GetTilemapCell(0, 1, 6);
        cell[0] = 0xd000;
        cell[1] = 0xd000;
        p->timedStatusIconVisible = 1;
    } else if (p->timedStatusIconVisible) {
        p->controlAnimation.flags |= 1;
        cell = Hud_GetTilemapCell(0, 1, 6);
        cell[0] = 0xd000;
        cell[1] = 0xd000;
        p->timedStatusIconVisible = 0;
    }
}

void Hud_PlayActionChangeCue(struct SceneHudState *p, u32 group, u32 mode, u32 selection)
{
    if (gEngineFlags & 0x1000) {
        if (group == 0 && mode == 0 && selection - 1 <= 2 && selection != p->previousCueSelection)
            Sound_PlaySong(0x21C);
    } else {
        if (p->actionCueInitialized == 0) {
            if (p->actionCueSuppressed == 0)
                Sound_PlaySong(0x21C);
            p->actionCueInitialized = 1;
        } else if (group == 0) {
            if (mode == 0 || (mode == 1 && selection == 0)) {
                if (selection != p->previousCueSelection)
                    Sound_PlaySong(0x21C);
            }
        }
    }
    p->previousCueGroup = group;
    p->previousCueSelection = selection;
}

static inline u32 TestFlags(u32 mask, u32 *p)
{
    return mask & *p;
}

static inline void SetDigitVisibility(struct SceneHudState *p, u32 mask, s32 hide)
{
    u32 *flags = &p->countdownDigits[0].flags;
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

void Hud_UpdateCountdownDigits(struct SceneHudState *p)
{
    s32 value, hundreds, tens, ones, base;
    u8 *res;
    if (p->countdownEnabled) {
        if (!p->actionOwner || TestFlags(4, &gUnk_02000560)) {
            SetDigitVisibility(p, 1, 1);
            p->countdownEnabled = 0;
        } else {
            value = *p->countdownValue;
            if (value > 59940) {
                SetDigitVisibility(p, 1, 1);
                p->actionCueInitialized = 0;
            } else {
                res = p->widgetResource;
                SetDigitVisibility(p, ~1, 0);
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
                    HideCountdownDigit(&p->countdownDigits[2]);
                    if (hundreds == 0 && tens == 0) {
                        Anim_LoadKeyframe(&p->countdownDigits[0], res, (u16)(0x33), 1);
                        Anim_LoadKeyframe(&p->countdownDigits[1], res, (u16)(ones + 0x33), 1);
                    } else {
                        Anim_LoadKeyframe(&p->countdownDigits[0], res, (u16)(tens + 0x29), 1);
                        Anim_LoadKeyframe(&p->countdownDigits[1], res, (u16)(ones + 0x29), 1);
                    }
                } else {
                    if (hundreds == 0 && tens == 0) {
                        Anim_LoadKeyframe(&p->countdownDigits[0], res, (u16)(0x33), 1);
                        Anim_LoadKeyframe(&p->countdownDigits[1], res, (u16)(0x33), 1);
                        Anim_LoadKeyframe(&p->countdownDigits[2], res, (u16)(ones + 0x33), 1);
                    } else {
                        Anim_LoadKeyframe(&p->countdownDigits[0], res, (u16)(hundreds + 0x29), 1);
                        Anim_LoadKeyframe(&p->countdownDigits[1], res, (u16)(tens + 0x29), 1);
                        Anim_LoadKeyframe(&p->countdownDigits[2], res, (u16)(ones + 0x29), 1);
                    }
                }
                Hud_PlayActionChangeCue(p, hundreds, tens, ones);
            }
        }
    }
    if (p->actionCueSuppressed)
        p->actionCueSuppressed--;
}

void Hud_DrawDecimalDigits(s32 value, s32 div, s32 x, s32 y, s32 flag)
{
    u32 base = 0x31;
    u16 *cell;
    if (!flag) {
        base = 0x21;
    }
    cell = Hud_GetTilemapCell(0, x, y);
    while (div > 0) {
        s32 d = Div(value, div);
        u16 t = d + base;
        *cell++ = t | 0xF000;
        do {
            value -= div * d;
        } while (0);
        div = Div(div, 10);
    }
}

void Hud_FillDigitCells(s32 count, u32 x, u32 y, s32 sign)
{
    s32 tile = 1;
    u16 *p;
    s32 i;
    if (sign >= 0) {
        tile = 0x30;
        if (sign == 0)
            tile = 0x20;
    }
    p = Hud_GetTilemapCell(0, x, y);
    for (i = 0; i < count; i++)
        *p++ = (u16)(tile | 0xF000);
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

void Hud_DrawDefaultResourceCounts(s32 a, s32 b, s32 c, s32 d, s32 e)
{
    Hud_DrawActiveStatValue(a, b, c, d, e, 0);
}

void Hud_FillTileRun(s32 count, u32 x, u32 y)
{
    u16 *p = Hud_GetTilemapCell(0, x, y);
    s32 i;
    for (i = 0; i < count; i++)
        *p++ = 0xF090;
}

void Hud_DrawCounter7B0(s32 a, s32 b)
{
    u16 *p = Hud_GetTilemapCell(0, a - 1, b);
    *p = 0xF001;
    if (gGameState->unk7B0 > 9999)
        gGameState->unk7B0 = 9999;
    Hud_DrawDecimalDigits(gGameState->unk7B0, 1000, a, b, 0);
}

void Hud_DrawCounter50C(s32 a, s32 b)
{
    u16 *p = Hud_GetTilemapCell(0, a - 1, b);
    *p = 0xF001;
    if (gGameState->menuRewardBalance > 9999)
        gGameState->menuRewardBalance = 9999;
    Hud_DrawDecimalDigits(gGameState->menuRewardBalance, 1000, a, b, 1);
}

void Hud_DrawHeaderCounter(void)
{
    u16 *p = Hud_GetTilemapCell(0, 0x18, 1);
    *p = 0xF001;
    if (gGameState->unk77C > 9999)
        gGameState->unk77C = 9999;
    Hud_DrawDecimalDigits(gGameState->unk77C, 1000, 0x19, 1, 0);
}

void Hud_DrawSelectedCounter(struct SceneHudState *self)
{
    u16 *p = Hud_GetTilemapCell(0, 0x12, 1);
    u8 mode = self->clockSelection;

    if (mode == 0) {
        *p++ = 0xF023;
        *p++ = 0xF022;
        *p++ = 0xF024;
        p[0] = 0xF09B;
        p[1] = 0xF001;
        if (gGameState->unk7B0 > 9999)
            gGameState->unk7B0 = 9999;
        Hud_DrawCounter7B0(0x17, 1);
    } else if (mode == 1) {
        *p++ = 0xF025;
        *p++ = 0xF026;
        *p++ = 0xF022;
        p[0] = 0xF09B;
        p[1] = 0xF030;
        if (gGameState->menuRewardBalance > 9999)
            gGameState->menuRewardBalance = 9999;
        Hud_DrawCounter50C(0x17, 1);
    } else {
        *p++ = 0xF0B9;
        *p++ = 0xF0BA;
        *p++ = 0xF022;
        p[0] = 0xF0BB;
        p[1] = 0xF09B;
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
    u16 *p = Hud_GetTilemapCell(0, x, y);
    *p = 0xF07C;
    p++;
    *p = 0xF07D;
    p[1] = 0xF07F;
}

void Hud_DrawClockLabelA(u32 x, u32 y)
{
    u16 *p = Hud_GetTilemapCell(0, x, y);
    *p = 0xF07C;
    p++;
    *p = 0xF07D;
    p[1] = 0xF07E;
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

void Hud_DrawStatusMetersAndCounts(struct SceneHudState *p, s32 b, s32 c)
{
    Hud_DrawDefaultHealthMeter(p, 1, b, c);
    c += 2;
    Hud_DrawDefaultEnergyMeter(p, 1, b, c, p->actionOwner->currentStatB,
                               p->actionOwner->maximumStatB);
    Hud_DrawDefaultResourceCounts(8, 4, 8, 3, 0);
}
