#include "stacked_actor.h"
#define StackedActor_WaitForPlayerRelease sub_08052E80
void StackedActor_WaitForPlayerRelease(struct StackedActor *p)
{
    p->frame++;
    if (p->frame > 0x3b)
        sub_08052628(p, sub_08052EAC);
}
