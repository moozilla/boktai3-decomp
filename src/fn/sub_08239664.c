#include "global.h"
#ifdef NONMATCHING
// fuzzy: 65.1%
// The shared 0x1F constant lands in ip and is rematerialized as in the ROM. Differences: the
// ROM re-zero-extends the colour with lsls #16 before the >>5 and >>10 shifts (a u32 colour
// with (u16) casts gets that but costs allocation elsewhere), and the +0xAC6 pointer stays in r1
// until the brightening loops copy it to r8.
extern u8 *gUnk_030042E4;

static inline u32 Brighten(u16 color, u8 *level)
{
    s32 r, g, b;
    r = (color & 0x1F) + *level;
    if (r > 0x1F)
        r = 0x1F;
    g = ((color >> 5) & 0x1F) + *level;
    if (g > 0x1F)
        g = 0x1F;
    b = ((color >> 10) & 0x1F) + *level;
    if (b > 0x1F)
        b = 0x1F;
    return (b << 10) | (g << 5) | r;
}

void sub_08239664(u8 *p)
{
    u8 **base = &gUnk_030042E4;
    u16 *src = (u16 *)(*base + ((u16 *)(p + 0xAB0))[*(s16 *)(p + 0xAC4)] * 32);
    u8 *amount = p + 0xAC6;
    u16 *dst;
    s32 i;

    if (*amount == 0) {
        if (p[0x419] == 0) {
            i = 0;
            dst = (u16 *)(p + 0x360);
            for (; i <= 15; src++, dst++, i++) {
                if ((u32)(i - 5) > 1 && i != 13)
                    *dst = *src;
            }
        } else {
            dst = (u16 *)(p + 0x360);
            for (i = 15; i >= 0; i--)
                *dst++ = *src++;
        }
    } else if (p[0x419] == 0) {
        i = 0;
        dst = (u16 *)(p + 0x360);
        for (; i <= 15; src++, dst++, i++) {
            if ((u32)(i - 5) > 1 && i != 13)
                *dst = Brighten(*src, amount);
        }
    } else {
        dst = (u16 *)(p + 0x360);
        for (i = 15; i >= 0; i--)
            *dst++ = Brighten(*src++, amount);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08239664);
#endif
