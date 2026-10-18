#include "global.h"

void SceneActorEffects_ReleaseResources(void);

s32 Dog_ReleaseResources(void)
{
    SceneActorEffects_ReleaseResources();
    return 0;
}
