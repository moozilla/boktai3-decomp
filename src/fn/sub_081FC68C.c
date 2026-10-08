#include "global.h"
struct S { u8 p0[0x9c]; u32 w9c; u8 p1[0xa2 - 0xa0]; u8 ba2; u8 p2[0x175 - 0xa3]; u8 b175; };
void sub_081FC68C(struct S *p)
{

    u32 t;
    p->w9c = (p->w9c & ~2) | 2;
    t = p->b175;
    p->b175 = t + 0x10;
    p->ba2 = -(t + 0x70);
}
