#include "global.h"

struct P { u8 f[0x20]; u8 a[0xf08 - 0x20]; u16 v; u8 g[0xfa8 - 0xf0a]; u8 h[1]; };
extern const u8 gUnk_0824DEB8[];
extern const u16 gUnk_0824DF28[];
void sub_08219728(u8 *, u8 *, u32);

void sub_0805F0A4(struct P *p, s32 a, s32 b)
{
    p->v = ((gUnk_0824DEB8[b - 1] + a) << 3) + 8;
    sub_08219728(p->h, p->a, gUnk_0824DF28[a]);
}
