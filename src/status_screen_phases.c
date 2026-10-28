#include "status_screen.h"
#include "render.h"
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
        p->noise = StatusScreen_UpdateScanlineNoise;
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

static inline int StatusScreen_MapWidth(int bg)
{
    return gBgRecords[bg].width;
}

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
        p->noise = StatusScreen_UpdateScanlineNoise;
    }
    if (p->timer == 48) {
        u16 *tiles = StatusScreen_GetTilePtr(0, 0, 0);
        int row, col;
        for (row = 0; row < 15; row++) {
            for (col = 0; col < StatusScreen_MapWidth(0) * 2; col++)
                *tiles++ = 0xD000;
        }
    }
    {
        register struct StatusScreen *phase asm("r4") = p;
        if (phase->timer++ >= phase->times[3]) {
            StatusScreenFn state = StatusScreen_BeginClose;
            phase->state = state;
            phase->timer = 0;
        }
        phase->dirty |= 15;
    }
}

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
            p->noise = StatusScreen_UpdateScanlineNoise;
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
