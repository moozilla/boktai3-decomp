#include "global.h"

void Palette_InterpolateBanks(u16 *dst, u16 *a, u16 *b, s32 weight, s32 shift)
{
    s32 max = 1 << shift;
    s32 i;

    if (weight == 0) {
        for (i = 15; i >= 0; i--)
            *dst++ = *a++;
    } else if (weight >= max) {
        for (i = 15; i >= 0; i--)
            *dst++ = *b++;
    } else {
        max -= weight;
        for (i = 15; i >= 0; i--) {
            s32 r = ((*a & 0x1f) * max + (*b & 0x1f) * weight) >> shift;
            s32 g = ((*a & 0x3e0) * max + (*b & 0x3e0) * weight) >> shift;
            s32 bl = ((*a & 0x7c00) * max + (*b & 0x7c00) * weight) >> shift;
            *dst = (r & 0x1f) | (g & 0x3e0) | (bl & 0x7c00);
            a++;
            b++;
            dst++;
        }
    }
}
