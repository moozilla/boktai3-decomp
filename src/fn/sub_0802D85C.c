#include "global.h"
void Shadow_Destroy(void *);
void ScaledShadow_Destroy(void *);
u32 Actor_DestroyShadow(u8 *p)
{
    u8 v = p[0x218];
    if (v == 1)
        Shadow_Destroy(p + 0x244);
    else if (v == 2)
        ScaledShadow_Destroy(p + 0x244);
    return 0;
}
