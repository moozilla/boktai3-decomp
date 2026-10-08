#include "global.h"
struct E { u8 p[0x38]; };
struct S { u8 p0[0x100]; struct E e[12]; };
void sub_08217EAC(void *);
void sub_081FE02C(struct S *p)
{
    s32 i;
    struct E *e = p->e;
    for (i = 11; i >= 0; i--) { sub_08217EAC(e); e++; }
}
