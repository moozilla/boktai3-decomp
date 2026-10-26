#include "global.h"
#include "ram.h"
void SpritePool_SetFrame(u8 *p, u32 v)
{
    if (gSpritePoolManager != 0)
        *(u16 *)(p + 0x1c) = v;
}
