#include "global.h"
void SceneActor_UpdateBehaviorCallbacks(void);
u32 Vorn_OnRender(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}
