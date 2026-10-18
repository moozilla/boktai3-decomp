#include "global.h"

extern u8 *gUnk_02000580;
void SceneActor_UpdateActiveCallbacks(u8 *);

u32 Lun_OnPlayerTargetInit(u8 *p)
{
    *(u8 **)(p + 0xdc) = gUnk_02000580 + 0x24;
    SceneActor_UpdateActiveCallbacks(p);
    return 0;
}
