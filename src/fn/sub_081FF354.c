#include "global.h"

void SpriteParticle_SetMotionParams(void *params, u32 flags, u32 lifetime, u32 direction, u32 speed)
{
    u8 *p = params;
    *(u16 *)(p + 0) = flags;
    *(u16 *)(p + 2) = lifetime;
    *(u8 *)(p + 6) = direction;
    *(u32 *)(p + 8) = speed;
}
