#include "global.h"
struct P { u8 f0[0x33C]; u16 a; s16 b; u8 x; u8 c; u8 e; u8 f; u8 g[2]; u16 d; };
void sub_08162E7C(struct P *p)
{
    p->a++;
    p->b -= 3;
    if (p->b <= 0x60) {
        p->b = 0x60;
        p->c = 3;
    }
}
