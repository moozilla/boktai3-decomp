#include "global.h"
struct S { u16 h0; u16 h2; u8 p4[2]; u8 b6; u8 b7; u32 w8; u8 pc[4]; u16 h10; u16 h12; u16 h14; u8 p16[2]; u16 h18; u16 h1a; u16 h1c; };
void sub_08219DD8(void *, u32);
void sub_082001BC(void *, void (*)(void));
void sub_081FF454(void);
void sub_082001C0(struct S *p)
{
    u32 a;
    sub_08219DD8(p, 0x20);
    sub_082001BC(p, sub_081FF454);

    p->h0 = 0;
    p->h2 = 0x3c;
    p->w8 = 5;
    p->b7 = p->b6 = 0;
    { u32 c = 0x1e; p->h10 = c; p->h12 = 0xf; p->h14 = c; p->h18 = c; p->h1a = c; p->h1c = c; }
}
