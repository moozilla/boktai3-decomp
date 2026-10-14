#include "background_flash.h"
#define BackgroundFlash_BeginDisappearance sub_08047B78
#define BackgroundFlash_SetState           sub_080476F4
void BackgroundFlash_SetState(struct BackgroundFlashController *, u32);
void BackgroundFlash_BeginDisappearance(struct BackgroundFlashController *controller)
{
    if (controller && controller->state)
        BackgroundFlash_SetState(controller, 4);
}
