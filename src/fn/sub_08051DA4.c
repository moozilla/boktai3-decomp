#include "global.h"
struct E { u8 f[0x30]; u16 v; u8 pad[2]; };
struct P { u8 f[0xb4]; u32 fb4; u32 fb8; u8 fbc[4]; struct E e[8]; };
void sub_08051A3C(struct E *, u32);
void sub_0824923C(struct P *, u32);
s32 sub_08051DA4(struct P *p)
{
    s32 i = 0;
    do {
        if (p->e[i].v != 0)
            sub_08051A3C(&p->e[i], p->fb8);
        i++;
    } while (i <= 7);
    sub_0824923C(p, p->fb4);
    return 0;
}
