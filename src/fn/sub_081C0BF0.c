#include "global.h"
struct S { u8 f[0x60]; u32 fl; u8 g[4]; u32 a68; u32 a6c; u32 a70; u32 a74; u8 h[0x454 - 0x78]; u32 a454; };
extern const u32 gUnk_0861215C[];
void sub_081C0BF0(struct S *p, u32 i)
{
    u32 t;
    p->a68 = i;
    p->a6c = 0;
    p->a454 = gUnk_0861215C[i];
    { u32 m = 1; t = p->fl; t |= m; }
    p->a70 = 0;
    p->a74 = 0;
    { u32 m = 2; t |= m; }
    p->fl = t;
}
