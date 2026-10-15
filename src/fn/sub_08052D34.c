#include "stacked_actor.h"
#define StackedActor_WaitForScriptParticles sub_08052D34
void StackedActor_WaitForScriptParticles(struct StackedActor *p)
{
    p->frame++;
    if (p->frame > 0xf)
        sub_08052628(p, sub_08052D60);
}
