#include "stacked_actor.h"
void StackedActor_RequestRetraction(void)
{
    if (gUnk_02000118)
        StackedActor_SetUpdate(gUnk_02000118, StackedActor_RetractToIdle);
}
