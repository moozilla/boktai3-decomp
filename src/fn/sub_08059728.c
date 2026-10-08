#include "global.h"

struct B { u8 f[0x1f]; u8 v; };
struct S { u8 f[0x6b0]; struct B *b; };

u8 sub_08059728(struct S *p)
{
    return p->b->v;
}
