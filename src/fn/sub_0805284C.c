#include "stacked_actor.h"
#define StackedActor_WaitForParticles sub_0805284C
void StackedActor_WaitForParticles(struct StackedActor *p)
{
    p->frame++;
    if (p->frame > 0xf)
        sub_08052628(p, sub_08052878);
}
