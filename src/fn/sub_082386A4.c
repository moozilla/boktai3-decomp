#define PlayerLink_IsActionModeAllowed sub_082386A4
#define gPlayerLinkSelection gUnk_020004B4
#include "global.h"
#include "area_effect_link.h"
static inline u32 PlayerLink_GetActionMode(void)
{
    if (gUnk_020004B4 == 0 || gUnk_020004B4->unavailable)
        return 0;
    return gUnk_020004B4->incomingPayload[0x3c];
}
s32 sub_082386A4(void)
{
    u32 mode = PlayerLink_GetActionMode();
    if (mode == 4 || mode == 5 || mode == 0)
        return 1;
    return 0;
}
