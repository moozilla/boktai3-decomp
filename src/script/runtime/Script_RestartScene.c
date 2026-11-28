#include "global.h"
#include "engine/runtime.h"
s32 SceneLifecycle_IsTransitionBlocked(void);
s32 Script_SeekToKeyword(s32);
s32 Script_RestartScene(void)
{
    if (SceneLifecycle_IsTransitionBlocked() == 0) {
        gSceneTransitionFlags = 0x40;
        if (Script_SeekToKeyword('s'))
            gSceneTransitionFlags |= 0x10;
        else if (Script_SeekToKeyword('r'))
            gSceneTransitionFlags |= 0x100;
    }
    return 0;
}
