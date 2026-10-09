#include "global.h"
struct A { u8 p0[6]; u8 b6, b7, b8, b9; u8 pA[0x16c-0xa]; void (*fn)(void); u8 p170[0x18e - 0x170]; u8 b18e, b18f; };
void sub_0822B2F8(s32);
void sub_080F4A4C(void);
void sub_080F4A0C(struct A *p)
{
    p->b6 = 0x80;
    p->b8 = 0x7f;
    p->b9 = 1;
    p->b18e = 0xf8;
    p->b18f = 0x10;
    p->fn = sub_080F4A4C;
    sub_0822B2F8(0x16d);
}
