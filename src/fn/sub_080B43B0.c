#include "global.h"
void SceneActorEffects_ReleaseResources(void);
u32 Vorn_OnDestroy(void)
{
    SceneActorEffects_ReleaseResources();
    return 0;
}
