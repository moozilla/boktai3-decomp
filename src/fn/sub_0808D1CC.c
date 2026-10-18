#include "global.h"

void SceneActorEffects_ReleaseResources(void);

u32 Ghoul_Destroy(void)
{
    SceneActorEffects_ReleaseResources();
    return 0;
}
