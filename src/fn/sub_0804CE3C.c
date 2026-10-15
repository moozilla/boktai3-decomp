#include "tether_actor.h"
void TetherActor_SetState(struct TetherActor *, s32);
s32 TetherActor_SetEntry(struct TetherActor *, s32);
void TetherActor_UpdateSpecialEntryTransition(struct TetherActor *p)
{
    u16 *q;
    u32 z, v, m;
    if (p->state != 0xe)
        TetherActor_SetState(p, 0xe);
    q = &p->chargeCollider.flags;
    v = *q;
    m = 4;
    z = 0;
    m |= v;
    *q = m;
    p->heading = z;
    if (TetherActor_SetEntry(p, 9))
        TetherActor_SetState(p, 0xd);
}
