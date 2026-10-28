#include "global.h"
#include "area_effect_link.h"
u8 *LinkPlayer_GetSessionPlayerData(u8 *s)
{
    s32 i = *(s32 *)(s + 0x18);
    u8 *g = gLinkSession;
    u8 *r;
    if (g == 0 || g[0x53] != 0 || i < 0)
        r = 0;
    else
        r = g + (i * 68 + 0x1c0);
    return r;
}
