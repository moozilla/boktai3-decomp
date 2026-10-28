#include "global.h"
#include "runtime.h"
s32 SceneLifecycle_IsTransitionBlocked(void);
s32 Script_SeekToKeyword(s32);
s32 Script_RestartScene(void)
{
    u32 *g;
    if (SceneLifecycle_IsTransitionBlocked() == 0) {
        g = &gSceneTransitionFlags;
        *g = 0x40;
        if (Script_SeekToKeyword(0x73))
            *g |= 0x10;
        else if (Script_SeekToKeyword(0x72))
            *g |= 0x100;
    }
    return 0;
}
