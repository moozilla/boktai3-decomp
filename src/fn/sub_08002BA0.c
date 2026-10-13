#include "global.h"
static inline void copy32(const void *source, void *dest, s32 size)
{
    CpuSet(source, dest, (size / 4 & 0x1fffff) | 0x04000000);
}
void Palette_ScaleRange(u16 *dest, const u16 *source, u32 brightness, s32 first, s32 end)
{
    s32 count;
    brightness >>= 1;
    count = end - first;
    if (count) {
        if (brightness >= 32) {
            u16 *to;
            source += first;
            to = dest + first;
            copy32(source, to, count * 2);
        } else {
            const u16 *in = source + first;
            u16 *out;
            s32 i;
            dest += first;
            out = dest;
            if (count > 0) {
                u32 rbMask = 0x7C1F, rbProduct = 0xF83E0;
                i = count;
                do {
                    u32 color = *in;
                    u32 rb = ((color & rbMask) * brightness) & rbProduct;
                    u32 g = ((color & 0x03E0) * brightness) & 0x7C00;
                    *out = (rb | g) >> 5;
                    in++;
                    out++;
                } while (--i);
            }
        }
    }
}
