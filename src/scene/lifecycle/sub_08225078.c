#include "engine/engine_pause.h"
#include "engine/video/render.h"

void MusicSlots_StopAllAndRetainResumable(void);

void SceneLifecycle_PauseScene(void)
{
    if (!Engine_PauseBits(1)) {
        u32 *p;
        u32 f;
        MusicSlots_StopAllAndRetainResumable();
        f = 2;
        p = &gEnginePauseFlags;
        *p = *p | f;
    }
}
