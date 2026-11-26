#include "field/stacked_actor/stacked_actor.h"
void StackedActor_RequestRetraction(void)
{
    if (gStackedActor)
        StackedActor_SetUpdate(gStackedActor, StackedActor_RetractToIdle);
}
