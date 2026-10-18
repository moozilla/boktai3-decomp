#include "global.h"

void OrbitingParticles_Stop(u8 *p)
{
    if (p) {
        *(u32 *)(p + 0x1C) = *(u32 *)(p + 0x18);
    }
}
