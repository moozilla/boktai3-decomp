#include "stacked_actor.h"
void StackedActor_WaitForParticles(struct StackedActor *p)
{
    p->frame++;
    if (p->frame > 0xf)
        StackedActor_SetUpdate(p, StackedActor_EmitFallingParticles);
}
