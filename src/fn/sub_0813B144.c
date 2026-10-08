#include "global.h"

struct S0813B144 { u8 filler[0x41A]; u16 v; };

u16 sub_0813B144(struct S0813B144 *p, u16 m)
{
    return p->v & m;
}
