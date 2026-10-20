#include "global.h"
// Blend two RGB555 colors: (a * ((1 << shift) - weight) + b * weight) >> shift per channel.
void Palette_BlendColor(u16 *dst, u16 *a, u16 *b, int weight, int shift)
{
    int inv = (1 << shift) - weight;
    int r = (((*a) & 0x1f) * inv + ((*b) & 0x1f) * weight) >> shift;
    int g = (((*a) & 0x3e0) * inv + ((*b) & 0x3e0) * weight) >> shift;
    int bl = (((*a) & 0x7c00) * inv + ((*b) & 0x7c00) * weight) >> shift;
    *dst = (r & 0x1f) | (g & 0x3e0) | (bl & 0x7c00);
}
