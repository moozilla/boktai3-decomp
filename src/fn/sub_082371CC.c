#include "global.h"
u32 ActorAttachment_Create(u32, u32, u32, u32, u32, u32);
u32 SpiralTrail_CreateWithParticles(u32 a, u32 b, u32 c, u32 d, u32 e)
{
    return ActorAttachment_Create(a, b, c, d, e, 1);
}
