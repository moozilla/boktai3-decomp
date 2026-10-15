#include "stacked_actor.h"
void StackedActor_WaitForPlayerRelease(struct StackedActor *p)
{
    p->frame++;
    if (p->frame > 0x3b)
        StackedActor_SetUpdate(p, StackedActor_EmitRisingParticles);
}
