#include "global.h"

struct P { u8 f[0x2c4]; u32 a; u8 g[0x40]; u32 b; };
u32 sub_08074600(s32, s32);

void sub_080B1EEC(struct P *p)
{
    p->a = 0;
    p->b = sub_08074600(0x19, 0);
}
