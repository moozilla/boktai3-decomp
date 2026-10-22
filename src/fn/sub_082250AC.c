#include "global.h"
extern u32 gUnk_0300523C;
void SceneLifecycle_PauseScene(void);
void SceneLifecycle_ResumeScene(void);

void sub_082250AC(void)
{
    u32 flags = gUnk_0300523C;
    if (!(flags & ~2)) {
        if (!(flags & 2))
            SceneLifecycle_PauseScene();
        else
            SceneLifecycle_ResumeScene();
    }
}
