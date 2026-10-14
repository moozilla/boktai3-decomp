#include "contact_burst.h"
u32 ContactBurst_RandomParticleFrame(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    {
        s32 sample = ((const u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308];
        return Mod(sample >> 8, 3) + 29;
    }
}
