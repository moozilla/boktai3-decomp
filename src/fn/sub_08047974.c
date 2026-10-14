#include "background_flash.h"
u32 BackgroundFlash_TakeEntryFlag(struct BackgroundFlashController *);
void BackgroundFlash_EmitEffect(void *);
void BackgroundFlash_UpdateAlternating(struct BackgroundFlashController *controller)
{
    if (BackgroundFlash_TakeEntryFlag(controller))
        BackgroundFlash_ShowLayer();
    controller->page = 1 - controller->page;
    if (Mod(controller->frame, 10) == 0)
        BackgroundFlash_EmitEffect(controller->effectSourceB);
}
