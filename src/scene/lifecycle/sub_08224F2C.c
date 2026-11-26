#include "engine/engine_pause.h"
#include "engine/runtime.h"

u32 SceneLifecycle_IsTransitionBlocked(void)
{
    if (!Engine_PauseBits(0x400) && !gSceneInitWord1 && !gSceneInitWord0)
        return 0;
    return 1;
}
