#include "scene_panel.h"
void ScenePanel_BeginBackgroundBurst(void)
{
    if (gScenePanel) {
        gScenePanel->mode = 64;
        gScenePanel->shakeOffset = 0;
        gScenePanel->effectPhase = 1;
        Sound_PlaySong(297);
    }
}
