#include "global.h"
#include "ram.h"
/* Registry routine FB4A: 1 if the scene actor manager (gSceneActorListManager) exists. */
s32 SceneActorManager_Exists(void)
{
    if (gSceneActorListManager)
        return 1;
    return 0;
}
