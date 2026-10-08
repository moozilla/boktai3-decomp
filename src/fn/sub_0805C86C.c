#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Z { struct H a, b; };
struct G { u8 f[0x30]; s16 x; u16 y; s16 z; };
struct R { u8 f0[0x500]; s16 a; u8 f1[0x504 - 0x502]; s16 b; };
extern struct G *gUnk_02000580;
void sub_08225F88(struct Z *);
void sub_0805C86C(u8 *p)
{
    struct Z z;
    struct G *g = gUnk_02000580;
    z.a.lo = (u32)((g->x + *(s16 *)(p + 0x500)) << 15) >> 16;
    z.a.hi = g->y + 0x96;
    z.b.lo = (u32)((g->z + *(s16 *)(p + 0x504)) << 15) >> 16;
    sub_08225F88(&z);
}
