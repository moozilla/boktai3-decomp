#include "global.h"

extern u32 gUnk_02000058;

void ScaledShadowManager_OnTaskDestroy(void)
{
    gUnk_02000058 = 0;
}
