#include "global.h"
struct P { u8 f0[0x3060]; u16 a; u16 b; u8 f1[0x5c]; u16 c; u16 d; };
void sub_08166094(struct P *p, u32 a, u32 b)
{
    p->a = a;
    p->b = b;
    p->c = a;
    p->d = b + 8;
}
