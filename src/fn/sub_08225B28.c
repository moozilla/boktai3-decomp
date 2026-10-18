#include "global.h"

u32 SceneBody_SetSprite(u8 *p, u32 v)
{
    *(u32 *)(p + 0x2c) = v;
    return 1;
}
