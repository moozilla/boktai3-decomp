#include "global.h"
#include "render.h"
#include "scene_hud_display.h"

// Remaining differences: loop-counter init is emitted before the hoisted 0xFFFFD000 literal in two loops.
// confidence=low: transient gauges use two independent countdowns.
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 Div(s32, s32);
void StatDisplay_DrawTwoDigitCounter(s32);
void ResultPanel_ClearFiveTiles(void);
enum { GAUGE_TILE_PALETTE = 13 << 12 };

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
        StatDisplay_DrawTwoDigitCounter((s16)p->timedStateParameter);
    } else if (p->transientGaugeVisible) {
        cell = Hud_GetTilemapCell(0, 29, 1);
        for (n = 0; n < 14; n++) {
            *cell = GAUGE_TILE_PALETTE;
            cell--;
        }
        ResultPanel_ClearFiveTiles();
        p->transientGaugeVisible = 0;
    }
}
