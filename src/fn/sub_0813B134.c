#include "global.h"

struct S0813B134 { u8 filler[0x41A]; u16 v; };

void sub_0813B134(struct S0813B134 *p, u32 m)
{
    p->v |= m;
}
