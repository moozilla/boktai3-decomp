#include "stacked_actor.h"
void StackedActor_WaitForRetraction(struct StackedActor *p)
{
    p->frame++;
    if (p->frame > 0xf)
        StackedActor_SetUpdate(p, StackedActor_FinishPlayerInteraction);
}
