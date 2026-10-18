#include "global.h"

void Sound_ResumePersistentSongs(void);

void SceneLifecycle_ResumeScene(void)
{
    u32 *p;
    u32 m;
    Sound_ResumePersistentSongs();
    p = (u32 *)0x0300523C;
    m = ~2;
    *p = *p & m;
}
