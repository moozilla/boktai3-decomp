#include "global.h"
void Collectible_ApplyPickupReward(u8 *, u32);
void Collectible_StartDeath(void);
void Collectible_OnPickup(u8 *s, u32 arg)
{
    u32 *p = (u32 *)(s + 0x9c);
    u32 mask = 1;
    *p |= mask;
    Collectible_ApplyPickupReward(s, arg);
    *(u32 *)(s + 0xf4) = (u32)Collectible_StartDeath;
    s[0xf1] = mask;
}
