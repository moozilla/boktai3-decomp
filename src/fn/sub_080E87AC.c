#include "global.h"

void SceneActor_UpdateBehaviorCallbacks(void);

s32 Mimic_OnUpdate(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}
