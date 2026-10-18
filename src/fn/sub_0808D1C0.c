#include "global.h"

void SceneActor_UpdateBehaviorCallbacks(void);

u32 Ghoul_OnMessage(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}
