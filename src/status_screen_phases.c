#include "status_screen.h"
// MATCHING_CONCESSION: register-pinning
// The optimization override reproduces BeginClose and Reveal; it is not historical compiler evidence.

void StatusScreen_BeginClose(struct StatusScreen *p)
{
    if (!p->timer) {
        Sound_PlaySong(227);
        p->top = 116;
        p->bottom = 160;
        p->amplitude = 0;
        p->delta = 4;
        p->noise = sub_0800EF90;
    }
    if (p->timer++ >= p->times[4]) {
        p->dirty &= ~3;
        {
            register void (*state)(struct StatusScreen *) = StatusScreen_Close;
            do {
                p->state = state;
                p->timer = 0;
            } while (0);
        }
    }
    p->dirty |= 4;
}

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
void sub_0800E974(struct StatusScreen *p)
{
    u16 zero = p->timer;
    if (zero == 0) {
        Sound_PlaySong(225);
        p->start = 1;
        p->end = 5;
        p->blend = 0x1084;
        p->fade = zero;
        p->fadeStep = 4;
        p->effect = StatusScreen_FadeOutPalette;
        p->top = zero;
        p->bottom = 116;
        p->amplitude = zero;
        p->delta = 4;
        p->noise = sub_0800EF90;
    }
    if (p->timer == 48) {
        u16 *tiles = StatusScreen_GetTilePtr(0, 0, 0);
        int row = 0;
        for (; row < 15;) {
            int col = 0;
            int next = row + 1;
            for (; col < (gUnk_03004C30[0].width * 2); col++)
                *(tiles++) = 0xD000;

            do {
                row = next;
            } while (0);
        }
    }
    if ((p->timer++) >= p->times[3]) {
        {
            typeof(p->state) *q1 = &p->state;
            u8 z1 = 0;
            p->timer = z1;
            *q1 = StatusScreen_BeginClose;
        }
    }
    p->dirty |= 15;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0800E974);
#endif

void StatusScreen_WaitForClose(struct StatusScreen *p)
{
    if (!StatusScreen_CheckConfirm(p)) {
        if (p->timer++ >= p->times[2]) {
            {
                register void (*state)(struct StatusScreen *) = sub_0800E974;
                do {
                    p->state = state;
                    p->timer = 0;
                } while (0);
            }
            p->noise = StatusScreen_ClearScanlineNoise;
        }
        p->dirty |= 15;
    }
}

void StatusScreen_Reveal(struct StatusScreen *p)
{
    if (!StatusScreen_CheckConfirm(p)) {
        if (!p->timer) {
            Sound_PlaySong(225);
            p->top = 0;
            p->bottom = 160;
            p->amplitude = 256;
            p->delta = -2;
            p->noise = sub_0800EF90;
            p->blend = 0x1084;
            p->fade = 0;
            p->fadeStep = 4;
            p->effect = StatusScreen_FadeInPalette;
        }
        if (p->timer++ >= p->times[1]) {
            {
                register void (*state)(struct StatusScreen *) = StatusScreen_WaitForReveal;
                asm("" : "+r"(p) : "r"(state));
                p->state = state;
            }
            p->timer = 0;
        }
        p->dirty |= 15;
    }
}

void StatusScreen_WaitForReveal(struct StatusScreen *p)
{
    if (!StatusScreen_CheckConfirm(p)) {
        if (p->timer++ >= p->times[0]) {
            do {
                p->noise = StatusScreen_ClearScanlineNoise;
                {
                    register void (*state)(struct StatusScreen *) = StatusScreen_WaitForClose;
                    p->state = state;
                    p->timer = 0;
                }
            } while (0);
        }
        p->dirty |= 15;
    }
}

// CFLAGS: -O2 -mthumb-interwork -fno-expensive-optimizations
