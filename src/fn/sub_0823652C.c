#include "global.h"
#ifdef NONMATCHING
// fuzzy: 75.0%
// 4 differing instruction runs; first diff at +0x1E (movs r0, #0x1f vs adds r4, r1, #0)
struct S {
    u8 p0[0x48];
    u16 *src;
    u16 dest[16];
    u8 b6c, alpha;
};
void sub_0823652C(struct S *s)
{
    u16 *p = s->src;
    int bias = (16 - s->alpha) * 15;
    int i = 0;
    do {
        u32 c = *p;
        int r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
        int a = s->alpha;
        r = (r * a + bias) >> 4;
        g = (g * a + bias) >> 4;
        b = (b * a + bias) >> 4;
        s->dest[i] = (b << 10) | (g << 5) | r;
        p++;
        i++;
    } while (i <= 15);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823652C);
#endif
