#include "global.h"

void SpriteParticle_SetSpriteResource(u8 *p, u32 a, u32 b, u32 c)
{
    *(u32 *)(p + 0x20) = a;
    *(u32 *)(p + 0x24) = b;
    *(u32 *)(p + 0x28) = c;
}
