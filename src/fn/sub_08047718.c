#include "background_flash.h"
extern void *gUnk_02000580;
void sub_0815E710(void *, void *, u32, u32, u32, u32);
void BackgroundFlash_EmitEffect(void *source)
{
    if (gUnk_02000580)
        sub_0815E710(gUnk_02000580, source, 0, 0, 0, 0x1000);
}
