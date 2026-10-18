#include "global.h"

void SceneActor_UpdateBehaviorCallbacks(void);

s32 Mummy_OnInteractionUpdate(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}
