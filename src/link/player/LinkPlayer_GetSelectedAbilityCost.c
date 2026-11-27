#include "global.h"
u32 LinkPlayer_GetAbilityCostById(s32);
u32 LinkPlayer_GetSelectedAbilityCost(u8 *s)
{
    s8 *p = (s8 *)(s + 0x33c);
    if (*p < 0)
        return 0;
    return LinkPlayer_GetAbilityCostById(*p);
}
