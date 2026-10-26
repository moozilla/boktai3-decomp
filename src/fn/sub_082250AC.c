#include "global.h"
#include "render.h"
void SceneLifecycle_PauseScene(void);
void SceneLifecycle_ResumeScene(void);

void sub_082250AC(void)
{
    u32 flags = gEnginePauseFlags;
    if (!(flags & ~2)) {
        if (!(flags & 2))
            SceneLifecycle_PauseScene();
        else
            SceneLifecycle_ResumeScene();
    }
}
