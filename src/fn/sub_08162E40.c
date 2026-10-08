#include "global.h"
struct P { u8 f0[0x33C]; u16 a; s16 b; u8 f1[1]; u8 c; u8 f2[4]; u16 d; };
void sub_08162E40(struct P *p)
{
    p->a++;
    p->b += 0x1e;
    if (p->b > 0x7e) {
        p->b = 0x7f;
        p->c = 2;
    }
}
