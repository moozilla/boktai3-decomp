#include "global.h"
struct E { u8 p[0x34]; };
struct S { u8 p0[0x528]; struct E e[4]; };
void sub_08217EAC(void *);
void sub_0817A41C(struct S *p)
{
    s32 i;
    struct E *e = p->e;
    for (i = 3; i >= 0; i--) { sub_08217EAC(e); e++; }
}
