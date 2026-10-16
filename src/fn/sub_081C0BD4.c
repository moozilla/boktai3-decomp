#include "global.h"
extern s32 gUnk_02000480;
/* Registry routine FB4A: 1 if the scene actor manager (gUnk_02000480) exists. */
s32 SceneActorManager_Exists(void)
{
    if (gUnk_02000480)
        return 1;
    return 0;
}
