#include "global.h"

extern u32 gUnk_020001C4;
void RegistryF246_RemoveAnimations(void);

s32 RegistryF246_Destroy(void)
{
    RegistryF246_RemoveAnimations();
    gUnk_020001C4 = 0;
    return 1;
}
