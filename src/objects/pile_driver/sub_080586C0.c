#include "objects/pile_driver/scene_panel.h"
void PileDriver_BeginBackgroundBurst(void)
{
    if (gScenePanel) {
        gScenePanel->mode = 64;
        gScenePanel->shakeOffset = 0;
        gScenePanel->effectPhase = 1;
        Sound_PlaySong(297);
    }
}
