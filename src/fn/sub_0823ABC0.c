#include "global.h"
#include "area_effect_link.h"
u32 LinkPlayer_HasSlotValueAtLeast(u8 *s, s32 m)
{
    s32 i = *(s32 *)(s + 0x18);
    s32 r;
    if (((u8 *)gLinkSession) == 0 || i < 0)
        r = 0;
    else {
        u8 *q = ((u8 *)gLinkSession) + 0x5c;
        r = q[i];
    }
    if (r < m)
        return 0;
    return 1;
}
