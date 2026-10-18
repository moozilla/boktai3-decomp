#include "global.h"
u32 SpriteBurst_Create(u32, u32, u32, u32, u32);
u32 SceneMovingParticle_Create(u32 a, u32 b, u32 c, u32 d)
{
    return SpriteBurst_Create(a, b, c, d, 1);
}
