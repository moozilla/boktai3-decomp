#include "status_screen.h"
// MATCHING_CONCESSION: register-pinning

u16 *sub_0800E3E0(u32 bg, u32 x, s32 y)
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
    register int *values asm("r12");
    digits[0] = left > 9;
    digits[1] = left - digits[0] * 10;
    dst = (s16 *)sub_0800E3E0(0, 6, 11);
    row = 0;
    values = digits;
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
    dst = (s16 *)sub_0800E3E0(0, 10, 11);
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
    register int *values asm("r12");
    int left = SunClock_GetBaseHour(), right = SunClock_GetBaseMinute();
    digits[0] = Div(left, 10);
    digits[1] = left - digits[0] * 10;
    dst = (s16 *)sub_0800E3E0(0, 16, 11);
    row = 0;
    values = digits;
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
    dst = (s16 *)sub_0800E3E0(0, 20, 11);
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

INCLUDE_ASM("asm/nonmatching", sub_0800E5B8);

void sub_0800E77C(struct StatusScreen *s)
{
    s32 a, b, c;
    sub_082286E0(&a, &b, &c, sub_08228D7C());
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
        sub_082279A8(3, 4, 4, 4, 4, 65535, 0);
        {
            register void (*state)(struct StatusScreen *) asm("r1") = StatusScreen_FadeAndExit;
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
