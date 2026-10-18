#include "global.h"

struct S0813B3BC {
    u8 filler[0xB34];
    u16 v;
};

u16 Player_GetEventFlags(struct S0813B3BC *p, u16 m)
{
    return p->v & m;
}
