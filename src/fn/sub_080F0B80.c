#include "global.h"

void SceneActor_UpdateActiveCallbacks(void);

s32 Ivy_UpdateInteraction(void)
{
    SceneActor_UpdateActiveCallbacks();
    return 0;
}
