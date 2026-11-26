#include "global.h"
/* RadialBurstEffect_BlendSpritePalette_08236E4C: same code as RadialBurstEffect_BlendSpritePalette at another address (ported by relatives.py; provisional name) */
struct S {
    u8 p0[0x48];
    u16 *src;
    u16 dest[16];
    u8 b6c;
    u8 alpha;
};
void RadialBurstEffect_BlendSpritePalette_08236E4C(struct S *s)
{
    u16 *p = s->src;
    int bias = (16 - s->alpha) * 15;
    int i = 0;
    u32 mask = 31;
    do {
        u32 c = *p;
        int r = c & 31;
        int g = ((c << 16) >> 21) & mask;
        int b = ((c << 16) >> 26) & mask;
        int a = s->alpha;
        r = (bias + r * a) >> 4;
        g = (bias + g * a) >> 4;
        b = (bias + b * a) >> 4;
        s->dest[i] = (b << 10) | (g << 5) | r;
        p++;
        i++;
    } while (i <= 15);
}
