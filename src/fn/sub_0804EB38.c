#include "global.h"
#define AreaEffect_GetInitialParticleMode sub_0804EB38

extern u32 gUnk_030053F4;
s32 sub_082290DC(void);

s32 AreaEffect_GetInitialParticleMode(void)
{
    s32 r;
    if ((gUnk_030053F4 & 0x100) || sub_082290DC() != 4)
        r = 0;
    else
        r = 1;
    return r;
}
