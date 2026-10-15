#include "stacked_actor.h"
#define StackedActor_WaitForRetraction sub_08052F1C
void StackedActor_WaitForRetraction(struct StackedActor *p)
{
    p->frame++;
    if (p->frame > 0xf)
        sub_08052628(p, sub_08052F48);
}
