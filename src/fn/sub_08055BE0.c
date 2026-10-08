#include "global.h"

struct S { u8 f[0x1b8]; u8 m; };
void sub_08055BA8(struct S *);
void sub_08055BC8(struct S *);

void sub_08055BE0(struct S *p)
{
    if (p->m == 0)
        sub_08055BA8(p);
    else
        sub_08055BC8(p);
}
