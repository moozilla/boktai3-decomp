#include "engine/video/render.h"

void Sound_ResumePersistentSongs(void);

void SceneLifecycle_ResumeScene(void)
{
    u32 *p;
    u32 m;
    Sound_ResumePersistentSongs();
    p = &gEnginePauseFlags;
    m = ~2;
    *p = *p & m;
}
