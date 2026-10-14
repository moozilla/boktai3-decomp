#include "background_flash.h"
void BackgroundFlash_SetState(struct BackgroundFlashController *, u32);
void BackgroundFlash_BeginAlternating(struct BackgroundFlashController *controller)
{
    if (controller)
        BackgroundFlash_SetState(controller, 3);
}
