#include "contact_burst.h"
u32 ContactBurst_RandomParticleFrame(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        s32 sample = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
        return Mod(sample >> 8, 3) + 29;
    }
}
