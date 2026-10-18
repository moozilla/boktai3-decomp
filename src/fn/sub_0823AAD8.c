#include "global.h"
s32 LinkPlayer_GetSelectedAbilityCost(void *);
u32 LinkPlayer_HasAbilityEnergy(u8 *s)
{
    s32 r = LinkPlayer_GetSelectedAbilityCost(s);
    s32 a = *(u16 *)(s + 0x428);
    if (a >= r)
        return 1;
    return 0;
}
