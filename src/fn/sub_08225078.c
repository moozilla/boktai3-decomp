#include "global.h"

void MusicSlots_StopAllAndRetainResumable(void);

void SceneLifecycle_PauseScene(void)
{
    u32 m = 1;
    if (((*(u32 *)0x030053F4 | *(u32 *)0x030053F0) & m) == 0) {
        u32 *p;
        u32 f;
        MusicSlots_StopAllAndRetainResumable();
        f = 2;
        p = (u32 *)0x0300523C;
        *p = *p | f;
    }
}
