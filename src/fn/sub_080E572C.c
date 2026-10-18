#include "global.h"

void SceneActor_UpdateBehaviorCallbacks(void);

s32 Dog_OnAuxiliaryUpdate(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}
