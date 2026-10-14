#include "background_flash.h"
#define BackgroundFlash_UpdateInactive sub_08047884
#define BackgroundFlash_TakeEntryFlag  sub_08047700
u32 BackgroundFlash_TakeEntryFlag(struct BackgroundFlashController *);
void BackgroundFlash_UpdateInactive(struct BackgroundFlashController *controller)
{
    if (BackgroundFlash_TakeEntryFlag(controller))
        BackgroundFlash_HideLayer();
}
