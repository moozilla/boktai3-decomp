#include "global.h"
void ActorEffect_SpawnBurstAtAnchor(u8 *, s32);

void sub_081EC5F8(u8 *p, s32 timer)
{
    if (!(timer & 15))
        ActorEffect_SpawnBurstAtAnchor(p, 0);
    if (!((timer + 3) & 15))
        ActorEffect_SpawnBurstAtAnchor(p, 1);
}
