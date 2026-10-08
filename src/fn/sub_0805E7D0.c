#include "global.h"

struct S { u8 f[7]; u8 v; };

u32 sub_0805E7D0(struct S *p, u32 m)
{
    return p->v & m;
}
