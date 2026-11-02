#include "departure_burst.h"

void sub_081EC5F8(struct DepartureActor *p, s32 timer)
{
    if (!(timer & 15))
        ActorEffect_SpawnBurstAtAnchor(p, 0);
    if (!((timer + 3) & 15))
        ActorEffect_SpawnBurstAtAnchor(p, 1);
}
