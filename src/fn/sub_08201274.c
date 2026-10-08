#include "global.h"
struct S { u16 h0; u16 h2; u8 p4[2]; u8 b6; u8 b7; u32 w8; u8 pc[4]; u16 h10; u16 h12; u16 h14; u8 p16[2]; u16 h18; u16 h1a; u16 h1c; };
void sub_08219DD8(void *, u32);
void sub_08201270(void *, void (*)(void));
void sub_082003BC(void);
void sub_08201274(struct S *p)
{
    sub_08219DD8(p, 0x20);
    sub_08201270(p, sub_082003BC);
    p->h0 = 0;
    p->h2 = 0x3c;
    p->w8 = 0x10;
    p->b6 = 0;
    { u32 c = 0x1e; p->h10 = c; p->h12 = c; p->h14 = c; p->h18 = c; p->h1a = 0xf; p->h1c = c; }
}
