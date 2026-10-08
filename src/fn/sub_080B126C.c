#include "global.h"

struct P { u8 f[0x6ba]; u16 a, b, c; u8 g[0x6b8 + 0x100 - 0x6c0]; };
struct Q { u8 f[0x6b8]; u8 v; };
s32 sub_0821ABA8(s32, s32);

void sub_080B126C(struct P *p)
{
    p->c = 0x78;
    p->b = 0x78;
    p->a = 0xf0;
    *((u8 *)p + 0x6b8) = sub_0821ABA8(0x68, -1);
}
