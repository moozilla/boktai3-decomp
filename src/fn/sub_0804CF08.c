#include "tether_actor.h"
void TetherActor_SetState(struct TetherActor *, u32);
s32 TetherActor_SetEntry(struct TetherActor *, u32);
void TetherActor_UpdateDirectedEntryTransition(struct TetherActor *p)
{
    u16 *q;
    u32 z, v, m;
    if (p->state != 0xF)
        TetherActor_SetState(p, 0xF);
    q = &p->chargeCollider.flags;
    v = *q;
    m = 4;
    z = 0;
    m |= v;
    *q = m;
    p->heading = z;
    if (TetherActor_SetEntry(p, 0x12))
        TetherActor_SetState(p, p->directedStep);
}
