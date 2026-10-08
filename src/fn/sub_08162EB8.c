#include "global.h"
struct P { u8 f0[0x33C]; u16 a; s16 b; u8 x; u8 c; u8 e; u8 f; u8 g[2]; u16 d; };
void sub_08162EB8(struct P *p)
{
    p->a += 2;
    p->b -= 3;
    if (p->b <= 0x40) {
        p->b = 0x40;
        p->c = 4;
    }
}
