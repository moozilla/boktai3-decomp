#include "effects/screen/background_flash.h"
/* BackgroundFlash_SetEffectSources: Sets both effect-source pointers when the controller is nonnull */
void BackgroundFlash_SetEffectSources(struct BackgroundFlashController *controller, void *sourceA,
                                      void *sourceB)
{
    if (controller) {
        controller->effectSourceA = sourceA;
        controller->effectSourceB = sourceB;
    }
}
