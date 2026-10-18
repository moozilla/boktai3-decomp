#include "global.h"
void SceneActor_UpdateBehaviorCallbacks(void);
u32 Spider_OnSecondaryUpdate(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}
