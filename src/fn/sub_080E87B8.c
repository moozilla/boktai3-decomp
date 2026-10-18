#include "global.h"

void SceneActorEffects_ReleaseResources(void);

s32 Mimic_ReleaseResources(void)
{
    SceneActorEffects_ReleaseResources();
    return 0;
}
