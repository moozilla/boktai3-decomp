#include "global.h"
u32 SpriteBurst_Create(u32, u32, u32, u32, u32);
u32 SceneMovingParticle_CreateWithOptions(u32 a, u32 b, u32 c, u32 d, u32 e)
{
    return SpriteBurst_Create(a, b, c, d, e);
}
