#include "background_flash.h"
#define BackgroundFlash_BeginAppearance sub_08047B58
#define BackgroundFlash_SetState        sub_080476F4
void BackgroundFlash_SetState(struct BackgroundFlashController *, u32);
void BackgroundFlash_BeginAppearance(struct BackgroundFlashController *controller)
{
    if (controller)
        BackgroundFlash_SetState(controller, 1);
}
