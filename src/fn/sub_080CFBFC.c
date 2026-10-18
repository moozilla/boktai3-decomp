#include "global.h"

void SceneActor_UpdateBehaviorCallbacks(void);

s32 Golem_OnSharedActorUpdate(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}
