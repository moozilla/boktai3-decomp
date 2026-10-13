#include "global.h"
#ifdef NONMATCHING
// fuzzy: 96.8%
// 2 differing instruction runs; first diff at +0x14 (adds r4, r0, #0 vs adds r5, r3, #0)
void sub_08221110(void *, void *, void *, int, int);
void sub_081A55D4(u8 *a, u8 *b, u8 *c, int d, int e, int n)
{
    if (n > 0) {
        u8 *z = c;
        u8 *y = b;
        int count = n;
        u8 *x = a;
        do {
            sub_08221110(x, y, z, d, e);
            z += 0x20;
            y += 0x20;
            x += 0x20;
        } while (--count);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A55D4);
#endif
