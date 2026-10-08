#include "global.h"

struct E { u8 f[0xfc]; };
struct P { u8 f0[0x18]; u16 n; u8 f1a[2]; struct E e[1]; };
void sub_0804FD70(struct E *, s32, s32, s32, s32, s32);
void sub_0804FDB4(struct E *, s32, s32);
void sub_0804FE20(struct E *);
void sub_0804FEA8(struct E *);

void sub_0804FEFC(struct P *p, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6)
{
    struct E *e = &p->e[p->n];
    sub_0804FD70(e, a1, a2, a3, a5, a6);
    sub_0804FDB4(e, a2, a4);
    sub_0804FE20(e);
    sub_0804FEA8(e);
}
