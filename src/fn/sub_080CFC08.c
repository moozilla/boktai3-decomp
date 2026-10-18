#include "global.h"

void SceneActorEffects_ReleaseResources(void);

s32 Golem_OnDestroy(void)
{
    SceneActorEffects_ReleaseResources();
    return 0;
}
