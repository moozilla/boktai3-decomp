#include "global.h"

void SceneActorEffects_ReleaseResources(void);

s32 Mummy_OnReleaseResources(void)
{
    SceneActorEffects_ReleaseResources();
    return 0;
}
