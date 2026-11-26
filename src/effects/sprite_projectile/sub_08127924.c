#include "engine/shared/global.h"
#include "engine/shared/ram.h"
u32 SpriteProjectile_InitPool(u8 *p)
{
    *(u32 *)(p + 0x860) = 0;
    gPooledSpriteManager = p;
    return 0;
}
