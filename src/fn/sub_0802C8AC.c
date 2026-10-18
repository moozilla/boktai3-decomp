#include "global.h"

void SceneBody_Init(u32, u32, u32, u32, u32, u32);
void Actor_InitEntity(u32 a, u32 b, u8 c, u32 d)
{
    SceneBody_Init(a, b, d, c, 7, a);
}
