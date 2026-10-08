#include "global.h"
struct S { u8 f0[0x48]; s16 a48; s16 a4a; s16 a4c; u8 g0[0xB39-0x4E]; u8 b39; u8 b3a; u8 f1; u8 b3c; u8 b3d; u8 b3e; u8 b3f; u8 b40; u8 b41; u8 b42; u8 b43; u8 f2; u8 b45; u8 g1[0x1124-0xB46]; s16 x; s16 y; s16 z;};
struct T { u8 f[0xB10]; s16 a; u8 g[2]; s16 b; u8 h[0xB32 - 0xB16]; u16 fl; };
extern const s16 gUnk_086149C4[];
void sub_0818AF78(struct T *p, s32 ang, s32 m)
{
    s32 v, w, r1, r2;
    p->fl |= 0x40;
    v = gUnk_086149C4[(ang + 0x40) & 0xff] * m;
    if (v >= 0) r1 = v >> 12;
    else r1 = -((-v) >> 12);
    p->a = r1;
    w = gUnk_086149C4[ang & 0xff] * m;
    if (w >= 0) r2 = w >> 12;
    else r2 = -((-w) >> 12);
    p->b = r2;
}
