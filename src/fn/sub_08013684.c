#include "global.h"
extern u32 gUnk_02000054;
void Effect_Remove(u8 *);
void ShadowList_Remove(u32, u8 *);
u32 Shadow_Destroy(u8 *p)
{
    Effect_Remove(p + 0xc);
    if (gUnk_02000054 != 0)
        ShadowList_Remove(gUnk_02000054, p);
    return 0;
}
