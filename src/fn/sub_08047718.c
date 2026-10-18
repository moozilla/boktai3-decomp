#include "background_flash.h"
extern void *gUnk_02000580;
void PlayerContact_ApplySyntheticContact(void *, void *, u32, u32, u32, u32);
void BackgroundFlash_EmitEffect(void *source)
{
    if (gUnk_02000580)
        PlayerContact_ApplySyntheticContact(gUnk_02000580, source, 0, 0, 0, 0x1000);
}
