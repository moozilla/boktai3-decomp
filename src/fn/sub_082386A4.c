#include "global.h"
#include "area_effect_link.h"
static inline u32 PlayerLink_GetActionMode(void)
{
    if (gLinkSession == 0 || gLinkSession->unavailable)
        return 0;
    return gLinkSession->incomingPayload[0x3c];
}
s32 PlayerLink_IsActionModeAllowed(void)
{
    u32 mode = PlayerLink_GetActionMode();
    if (mode == 4 || mode == 5 || mode == 0)
        return 1;
    return 0;
}
