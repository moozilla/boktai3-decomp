#include "background_flash.h"
void BackgroundFlash_SetState(struct BackgroundFlashController *, u32);
void BackgroundFlash_BeginDisappearance(struct BackgroundFlashController *controller)
{
    if (controller && controller->state)
        BackgroundFlash_SetState(controller, 4);
}
