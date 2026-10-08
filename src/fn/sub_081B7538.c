#include "global.h"

struct E { u8 f[0xfc]; u16 h; u8 g[2]; u8 a; u8 g2; u8 b; };
struct A { u8 f[0x5c]; struct E e[1]; };

void sub_081B7538(u8 *p, s32 i)
{
    struct E *e = (struct E *)(p + 0x5c + i * 0x118);
    if (e->b == 2) {
        e->a = 0;
        e->h = 0;
    }
}
