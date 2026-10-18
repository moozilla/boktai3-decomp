#include "background_flash.h"
/* BackgroundFlash_SetEffectSources: Sets both effect-source pointers when the controller is nonnull */
#define BackgroundFlash_SetEffectSources sub_08047B94
#define BackgroundFlash_SetEffectSources sub_08047B94
void BackgroundFlash_SetEffectSources(struct BackgroundFlashController *controller, void *sourceA,
                                      void *sourceB)
{
    if (controller) {
        controller->effectSourceA = sourceA;
        controller->effectSourceB = sourceB;
    }
}
