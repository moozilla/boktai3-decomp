#include "status_screen.h"

u16 *StatusScreen_GetTilePtr(u32 bg, u32 x, s32 y)
{
    struct BgMapInfo *t = &gUnk_03004C30[bg];
    u16 *p = t->map;
    p = p + x;
    return p + (t->width << 1) * y;
}

void NumericDisplay_DrawPair(void *unused, int left, int right)
{
    int digits[2];
    s16 *dst;
    int row, col;
    int *values;
    digits[0] = left > 9;
    digits[1] = left - digits[0] * 10;
    dst = (s16 *)StatusScreen_GetTilePtr(0, 6, 11);
    do {
        row = 0;
        values = digits;
    } while (0);
    for (; row < 2; row++) {
        int shift = row << 4;
        int *digit = values;
        s16 *out = dst;
        for (col = 0; col < 2; col++) {
            int tile = *digit++ + 32;
            *out++ = shift + tile | -12288;
        }
        dst += gUnk_03004C30[0].width * 2;
    }

    digits[0] = Div(right, 10);
    digits[1] = right - digits[0] * 10;
    dst = (s16 *)StatusScreen_GetTilePtr(0, 10, 11);
    row = 0;
    values = digits;
    for (; row < 2;) {
        int next = row + 1;
        row <<= 4;
        {
            int *digit = values;
            s16 *out = dst;
            for (col = 0; col < 2; col++) {
                int tile = *digit++ + 32;
                *out++ = row + tile | -12288;
            }
            dst += gUnk_03004C30[0].width * 2;
        }
        row = next;
    }
}

void NumericDisplay_DrawStatusValues(struct StatusScreen *unused)
{
    int digits[2];
    s16 *dst;
    int row, col;
    int *values;
    int left = SunClock_GetBaseHour(), right = SunClock_GetBaseMinute();
    digits[0] = Div(left, 10);
    digits[1] = left - digits[0] * 10;
    dst = (s16 *)StatusScreen_GetTilePtr(0, 16, 11);
    do {
        row = 0;
        values = digits;
    } while (0);
    for (; row < 2; row++) {
        int shift = row << 4;
        int *digit = values;
        s16 *out = dst;
        for (col = 0; col < 2; col++) {
            int tile = *digit++ + 32;
            *out++ = shift + tile | -12288;
        }
        dst += gUnk_03004C30[0].width * 2;
    }

    digits[0] = Div(right, 10);
    digits[1] = right - digits[0] * 10;
    dst = (s16 *)StatusScreen_GetTilePtr(0, 20, 11);
    row = 0;
    values = digits;
    for (; row < 2;) {
        int next = row + 1;
        row <<= 4;
        {
            int *digit = values;
            s16 *out = dst;
            for (col = 0; col < 2; col++) {
                int tile = *digit++ + 32;
                *out++ = row + tile | -12288;
            }
            dst += gUnk_03004C30[0].width * 2;
        }
        row = next;
    }
}

#ifdef NONMATCHING
// fuzzy: 65.9%
// scope shape 54 (differences once low registers are set aside)
void sub_0800E5B8(struct StatusScreen *unused)
{
    int digits[2];
    s16 *dst;
    int row, col, n, base;
    int *values;
    u32 hours, minutes;
    struct StatusGameState *c = &gUnk_03005430;
    if (!c)
        return;
    if (c->mode == 0 || c->mode == 4 || c->mode == 5) {
        dst = (s16 *)StatusScreen_GetTilePtr(0, 4, 15);
        hours = gUnk_03005430.altCounters[0];
        minutes = gUnk_03005430.altCounters[1];
    } else {
        dst = (s16 *)StatusScreen_GetTilePtr(0, 4, 15);
        hours = gUnk_03005430.counters[0];
        minutes = gUnk_03005430.counters[1];
    }
    for (row = 0; row < 2; row++) {
        int shift = row << 4;
        int tile = shift + 0x6a;
        s16 *out = dst;
        for (col = 0; col < 4; col++)
            *out++ = (tile + col) | -12288;
        dst += gUnk_03004C30[0].width * 2;
    }

    dst = (s16 *)StatusScreen_GetTilePtr(0, 8, 15);
    if (gUnk_03005430.mode == 0 || gUnk_03005430.mode == 4 || gUnk_03005430.mode == 5) {
        n = 2;
        base = 0x6e;
    } else {
        n = 2;
        base = 0x4e;
    }
    for (row = 0; row < 2; row++) {
        int shift = row << 4;
        s16 *out = dst;
        for (col = 0; col < n; col++)
            *out++ = (shift + base + col) | -12288;
        dst += gUnk_03004C30[0].width * 2;
    }

    digits[0] = Div(hours, 10);
    digits[1] = hours - digits[0] * 10;
    dst = (s16 *)StatusScreen_GetTilePtr(0, 16, 15);
    do {
        row = 0;
        values = digits;
    } while (0);
    for (; row < 2; row++) {
        int shift = row << 4;
        int *digit = values;
        s16 *out = dst;
        for (col = 0; col < 2; col++) {
            int tile = *digit++ + 0x60;
            *out++ = shift + tile | -12288;
        }
        dst += gUnk_03004C30[0].width * 2;
    }

    digits[0] = Div(minutes, 10);
    digits[1] = minutes - digits[0] * 10;
    dst = (s16 *)StatusScreen_GetTilePtr(0, 22, 15);
    row = 0;
    values = digits;
    for (; row < 2;) {
        int next = row + 1;
        row <<= 4;
        {
            int *digit = values;
            s16 *out = dst;
            for (col = 0; col < 2; col++) {
                int tile = *digit++ + 0x60;
                *out++ = row + tile | -12288;
            }
            dst += gUnk_03004C30[0].width * 2;
        }
        row = next;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0800E5B8);
#endif

void StatusScreen_RefreshDirtyDisplays(struct StatusScreen *s)
{
    s32 a, b, c;
    Clock_UnpackDate(&a, &b, &c, SunClock_GetPackedDate());
    if (s->dirty & 1)
        NumericDisplay_DrawPair(s, b, c);
    if (s->dirty & 2)
        NumericDisplay_DrawStatusValues(s);
    if (s->dirty & 4)
        sub_0800E5B8(s);
    s->dirty = 0;
}

int StatusScreen_CheckConfirm(struct StatusScreen *p)
{
    if ((gUnk_03005260[1] & 8) || (gUnk_03005260[1] & 1)) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 65535, 0);
        {
            register void (*state)(struct StatusScreen *) = StatusScreen_FadeAndExit;
            p->state = state;
        }
        p->timer = 0;
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
        {
            u16 *flags = &gUnk_03004BD8;
            s32 mask;
            mask = ~256;
            mask &= *flags;
            *flags = mask;
        }
    }
    {
        u32 flags = p->dirty;
        u32 result;
        result = 4;
        result |= flags;
        {
            u32 mask = 65532;
            p->dirty = result & mask;
        }
    }
    p->timer++;
}
