#include "objects/pile_driver/pile_driver.h"
void PileDriver_BeginBackgroundBurst(void)
{
    if (gScenePanel) {
        gScenePanel->mode = 64;
        gScenePanel->shakeOffset = 0;
        gScenePanel->effectPhase = 1;
        Sound_PlaySong(297);
    }
}
