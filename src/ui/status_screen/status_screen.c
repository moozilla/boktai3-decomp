#include "ui/status_screen/status_screen.h"
#include "engine/video/render.h"
#include "gba/io_reg.h"

u16 *StatusScreen_GetTilePtr(u32 bg, u32 x, s32 y)
{
    struct BackgroundLayer *layer = &gBgRecords[bg];
    u16 *map = layer->map;
    return map + x + (layer->width << 1) * y;
}

void NumericDisplay_DrawPair(void *unused, int left, int right)
{
    int digits[2];
    u16 *dst;
    int row, col;
    digits[0] = left > 9;
    digits[1] = left - digits[0] * 10;
    dst = StatusScreen_GetTilePtr(0, 6, 11);
    for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
            u16 *out = &dst[col];
            int tile = digits[col] + 32;
            *out = ((row << 4) + tile) | 0xD000;
        }
        dst += gBgRecords[0].width * 2;
    }

    digits[0] = Div(right, 10);
    digits[1] = right - digits[0] * 10;
    dst = StatusScreen_GetTilePtr(0, 10, 11);
    for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
            u16 *out = &dst[col];
            int tile = digits[col] + 32;
            *out = ((row << 4) + tile) | 0xD000;
        }
        dst += gBgRecords[0].width * 2;
    }
}

void NumericDisplay_DrawStatusValues(struct StatusScreen *unused)
{
    int digits[2];
    u16 *dst;
    int row, col;
    int hour = SunClock_GetBaseHour(), minute = SunClock_GetBaseMinute();
    digits[0] = Div(hour, 10);
    digits[1] = hour - digits[0] * 10;
    dst = StatusScreen_GetTilePtr(0, 16, 11);
    for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
            u16 *out = &dst[col];
            int tile = digits[col] + 32;
            *out = ((row << 4) + tile) | 0xD000;
        }
        dst += gBgRecords[0].width * 2;
    }

    digits[0] = Div(minute, 10);
    digits[1] = minute - digits[0] * 10;
    dst = StatusScreen_GetTilePtr(0, 20, 11);
    for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
            u16 *out = &dst[col];
            int tile = digits[col] + 32;
            *out = ((row << 4) + tile) | 0xD000;
        }
        dst += gBgRecords[0].width * 2;
    }
}

void StatusScreen_DrawClock(struct StatusScreen *unused)
{
    int digits[2];
    u16 *dst;
    int row, col, width, base;
    u32 hours, minutes;
    struct StatusGameState *clock = &gSunClock;
    if (!clock)
        return;
    if (clock->mode == 0 || clock->mode == 4 || clock->mode == 5) {
        dst = StatusScreen_GetTilePtr(0, 4, 15);
        hours = clock->altCounters[0];
        minutes = clock->altCounters[1];
    } else {
        dst = StatusScreen_GetTilePtr(0, 4, 15);
        hours = clock->counters[0];
        minutes = clock->counters[1];
    }
    for (row = 0; row < 2; row++) {
        base = 0x6a;
        for (col = 0; col < 4; col++)
            dst[col] = ((row << 4) + base + col) | 0xD000;
        dst += gBgRecords[0].width * 2;
    }

    dst = StatusScreen_GetTilePtr(0, 8, 15);
    if (clock->mode == 0 || clock->mode == 4 || clock->mode == 5) {
        width = 2;
        base = 0x6e;
    } else {
        width = 2;
        base = 0x4e;
    }
    for (row = 0; row < 2; row++) {
        for (col = 0; col < width; col++)
            dst[col] = ((row << 4) + base + col) | 0xD000;
        dst += gBgRecords[0].width * 2;
    }

    digits[0] = Div(hours, 10);
    digits[1] = hours - digits[0] * 10;
    dst = StatusScreen_GetTilePtr(0, 16, 15);
    for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
            u16 *out = &dst[col];
            int tile = digits[col] + 0x60;
            *out = ((row << 4) + tile) | 0xD000;
        }
        dst += gBgRecords[0].width * 2;
    }

    digits[0] = Div(minutes, 10);
    digits[1] = minutes - digits[0] * 10;
    dst = StatusScreen_GetTilePtr(0, 22, 15);
    for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
            u16 *out = &dst[col];
            int tile = digits[col] + 0x60;
            *out = ((row << 4) + tile) | 0xD000;
        }
        dst += gBgRecords[0].width * 2;
    }
}

enum { STATUS_DIRTY_DATE = 1, STATUS_DIRTY_BASE_TIME = 2, STATUS_DIRTY_CLOCK = 4 };

void StatusScreen_RefreshDirtyDisplays(struct StatusScreen *s)
{
    s32 a, b, c;
    Clock_UnpackDate(&a, &b, &c, SunClock_GetPackedDate());
    if (s->dirty & STATUS_DIRTY_DATE)
        NumericDisplay_DrawPair(s, b, c);
    if (s->dirty & STATUS_DIRTY_BASE_TIME)
        NumericDisplay_DrawStatusValues(s);
    if (s->dirty & STATUS_DIRTY_CLOCK)
        StatusScreen_DrawClock(s);
    s->dirty = 0;
}

int StatusScreen_CheckConfirm(struct StatusScreen *p)
{
    if ((gPadState[1] & 8) || (gPadState[1] & 1)) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 65535, 0);
        StatusScreen_SetState(p, StatusScreen_FadeAndExit);
        return 1;
    }
    return 0;
}

void StatusScreen_Close(struct StatusScreen *p)
{
    if (!p->timer) {
        p->start = 12;
        p->end = 16;
        p->blend = 0x1084;
        p->fade = 0;
        p->fadeStep = 2;
        p->effect = StatusScreen_FadeOutPalette;
    }
    if (p->timer == p->times[5]) {
        Script_Call(p->script, &p->args);
        Task_Kill(p);
        Bg_DisableDisplay(DISPCNT_BG0_ON);
    }
    p->dirty |= 4;
    p->dirty &= ~3;
    p->timer++;
}
