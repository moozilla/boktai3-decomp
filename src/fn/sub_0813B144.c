#include "global.h"

struct S0813B144 {
    u8 filler[0x41A];
    u16 v;
};

u16 Player_GetControlFlags(struct S0813B144 *p, u16 m)
{
    return p->v & m;
}
