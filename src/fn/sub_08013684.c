#include "global.h"
#include "ram.h"
void Effect_Remove(u8 *);
void ShadowList_Remove(u32, u8 *);
u32 Shadow_Destroy(u8 *p)
{
    Effect_Remove(p + 0xc);
    if (gShadowManager != 0)
        ShadowList_Remove(gShadowManager, p);
    return 0;
}
