#include "objects/pile_driver/pile_driver.h"
void PileDriver_BeginBackgroundBurst(void)
{
    if (gPileDriver) {
        gPileDriver->mode = 64;
        gPileDriver->shakeOffset = 0;
        gPileDriver->effectPhase = 1;
        Sound_PlaySong(297);
    }
}
