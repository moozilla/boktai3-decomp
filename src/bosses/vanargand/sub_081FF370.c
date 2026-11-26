#include "global.h"

void SpriteParticle_SetUpdateCallback(u8 *p, u32 a)
{
    *(u32 *)(p + 0xC) = a;
}
