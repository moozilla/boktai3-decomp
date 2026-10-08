#include "global.h"
struct E { u16 a; u16 b; u16 c; u16 d; };
void sub_08165B5C(u8 *p, s32 i, u16 a, u16 b)
{
    struct E *e = (struct E *)(i * 0x60 + (u32)p + 0x1440);
    e->a = a;
    e->b = b;
    e->c = 0;
    e->d = 0;
}
