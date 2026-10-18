#include "global.h"

s32 Player_GetEnchantmentEnergyCost(u8 *p);

s32 Player_CanAffordEnchantment(u8 *p)
{
    u8 *q = p;
    s32 r = Player_GetEnchantmentEnergyCost(p);

    q += 0x428;
    if (*(u16 *)q >= r) {
        return 1;
    }
    return 0;
}
