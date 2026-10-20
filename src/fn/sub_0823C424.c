#include "global.h"

s32 Div(s32, s32);

#ifdef NONMATCHING
// fuzzy: 83.3%
// Open: the original keeps a redundant second null check of p (inlined macro) before the flag test.

s32 sub_0823C424(u8 *p)
{
    s32 n, t;
    if (p != 0) {
        if (*(u16 *)(p + 0x538) != 0 && *(u16 *)(p + 0x534) == 1) {
            goto full;
        }
        t = (p != 0 && (*(u32 *)(p + 0x448) & 0x40)) ? 1 : 0;
        if (t != 0) {
            goto full;
        }
    }
    n = Div(0, 100);
    if (n > 16) {
    full:
        n = 16;
    } else if (n <= 8) {
        n = 9;
    }
    if (p[0x5B9] == 2 && p[0x5BB] != 0) {
        n -= 4;
    }
    if (p[0x4F6] != 0) {
        n -= p[0x4F6];
    }
    if (n <= 1) {
        return 2;
    }
    return n;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823C424);
#endif
