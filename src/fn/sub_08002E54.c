#include "global.h"

void sub_08002C38(void *, u8 *);
void sub_08002E00(void *, u8 *);
void sub_08002D58(void *, u8 *);
struct S { u8 f[0x4c]; u8 v; };

void sub_08002E54(struct S *p)
{
    u8 *q = &p->v;
    sub_08002C38(p, q);
    *q = 0x40;
    sub_08002E00(p, q);
    sub_08002D58(p, q);
}
