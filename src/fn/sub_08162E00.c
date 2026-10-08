#include "global.h"
struct P { u8 f0[0x33C]; u16 a; u16 b; u8 f1[1]; u8 c; u8 f2[4]; u16 d; };
void sub_0822B3E8(void);
void sub_0822B2F8(s32);
void sub_08162E00(struct P *p)
{
    sub_0822B3E8();
    sub_0822B2F8(2);
    sub_0822B2F8(0x12c);
    p->a = 10;
    p->b = 0x40;
    p->c = 1;
}
