#include "background_flash.h"
#define BackgroundFlash_UpdateAlternating sub_08047974
#define BackgroundFlash_TakeEntryFlag     sub_08047700
#define BackgroundFlash_EmitEffect        sub_08047718
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
