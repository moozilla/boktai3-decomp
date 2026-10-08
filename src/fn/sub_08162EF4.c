#include "global.h"
struct P { u8 f0[0x33C]; s16 a; u16 b; u8 f1[1]; u8 c; u8 f2[4]; u16 d; };
void sub_08162EF4(struct P *p)
{
    p->a += 3;
    if (p->a > 0x3f) {
        p->a = 0x40;
        p->c = 5;
        p->d = 0;
    }
}
