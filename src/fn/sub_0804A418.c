#include "global.h"
extern u8 *gUnk_02000710;
s32 TetherActor_GetConfiguredKind(void)
{
    return *(s16 *)(gUnk_02000710 + 0x5AA);
}
