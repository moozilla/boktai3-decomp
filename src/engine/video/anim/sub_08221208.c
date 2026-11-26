#include "global.h"

void Palette_BlendBank(u16 *dst, u16 *src, u16 color, s32 weight, s32 shift)
{
    s32 r = color & 0x1f;
    s32 g = color & 0x3e0;
    s32 b = color & 0x7c00;
    s32 max = 1 << shift;
    s32 i;

    if (weight == 0) {
        for (i = 15; i >= 0; i--)
            *dst++ = *src++;
    } else if (weight >= max) {
        for (i = 15; i >= 0; i--)
            *dst++ = color;
    } else {
        max -= weight;
        {
            s32 wr = r * weight;
            s32 wg = g * weight;
            s32 wb = b * weight;
            for (i = 15; i >= 0; i--) {
                s32 nr = ((*src & 0x1f) * max + wr) >> shift;
                s32 ng = ((*src & 0x3e0) * max + wg) >> shift;
                s32 nb = ((*src & 0x7c00) * max + wb) >> shift;
                *dst = (nr & 0x1f) | (ng & 0x3e0) | (nb & 0x7c00);
                src++;
                dst++;
            }
        }
    }
}
