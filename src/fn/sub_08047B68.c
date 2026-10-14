#include "background_flash.h"
#define BackgroundFlash_BeginAlternating sub_08047B68
#define BackgroundFlash_SetState         sub_080476F4
void BackgroundFlash_SetState(struct BackgroundFlashController *, u32);
void BackgroundFlash_BeginAlternating(struct BackgroundFlashController *controller)
{
    if (controller)
        BackgroundFlash_SetState(controller, 3);
}
