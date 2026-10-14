#include "background_flash.h"
u32 BackgroundFlash_TakeEntryFlag(struct BackgroundFlashController *);
void BackgroundFlash_EmitEffect(void *);
void BackgroundFlash_UpdateFlickering(struct BackgroundFlashController *controller)
{
    if (BackgroundFlash_TakeEntryFlag(controller))
        BackgroundFlash_ShowLayer();
    controller->page = 0;
    if (controller->stateFrame & 1)
        BackgroundFlash_ShowLayer();
    else
        BackgroundFlash_HideLayer();
    if (Mod(controller->frame, 10) == 0)
        BackgroundFlash_EmitEffect(controller->effectSourceA);
}
