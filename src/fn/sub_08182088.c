#include "global.h"

/* tentative: pulses palette entry 0 of two rows with a 64-frame triangle-wave brightness */
#define Palette_UpdatePulseFade sub_08182088

struct PalFade {
    u8 pad0[0xA];
    u16 src[0x100];
    u16 dst[0x100];
    u8 pad40A[0x488 - 0x40A];
    s16 counter;
    u8 idx;
};

void Palette_UpdatePulseFade(struct PalFade *p)
{
    s32 c, g, b;

    if (p->counter < 32) {
        c = p->src[(p->idx + 1) * 16];
        g = (c >> 5) & 31;
        b = (c >> 10) & 31;
        c = c & 31;
        c = (c * (32 - p->counter)) >> 5;
        g = (g * (32 - p->counter)) >> 5;
        b = (b * (32 - p->counter)) >> 5;
        p->dst[(p->idx + 1) * 16] = c | (g << 5) | (b << 10);
        c = p->src[(p->idx + 2) * 16];
        g = (c >> 5) & 31;
        b = (c >> 10) & 31;
        c = c & 31;
        c = (c * (32 - p->counter)) >> 5;
        g = (g * (32 - p->counter)) >> 5;
        b = (b * (32 - p->counter)) >> 5;
        p->dst[(p->idx + 2) * 16] = c | (g << 5) | (b << 10);
    } else {
        c = p->src[(p->idx + 1) * 16];
        g = (c >> 5) & 31;
        b = (c >> 10) & 31;
        c = c & 31;
        c = (c * (p->counter - 32)) >> 5;
        g = (g * (p->counter - 32)) >> 5;
        b = (b * (p->counter - 32)) >> 5;
        p->dst[(p->idx + 1) * 16] = c | (g << 5) | (b << 10);
        c = p->src[(p->idx + 2) * 16];
        g = (c >> 5) & 31;
        b = (c >> 10) & 31;
        c = c & 31;
        c = (c * (p->counter - 32)) >> 5;
        g = (g * (p->counter - 32)) >> 5;
        b = (b * (p->counter - 32)) >> 5;
        p->dst[(p->idx + 2) * 16] = c | (g << 5) | (b << 10);
    }
    p->counter++;
    if (p->counter > 63)
        p->counter = 0;
}
