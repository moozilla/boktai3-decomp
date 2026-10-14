#include "background_flash.h"
u32 BackgroundFlash_TakeEntryFlag(struct BackgroundFlashController *);
void BackgroundFlash_UpdateInactive(struct BackgroundFlashController *controller)
{
    if (BackgroundFlash_TakeEntryFlag(controller))
        BackgroundFlash_HideLayer();
}
