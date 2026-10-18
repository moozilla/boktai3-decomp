#include "global.h"

void SceneActor_UpdateActiveCallbacks(void);

s32 Octopus_UpdateInteraction(void)
{
    SceneActor_UpdateActiveCallbacks();
    return 0;
}
