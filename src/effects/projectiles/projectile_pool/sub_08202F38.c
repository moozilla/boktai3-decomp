// COMPILER: old_agbcc
#include "global.h"
/* ProjectileConfig_EnableSpriteFlag200: Enables the configuration option that adds flag 0x200 to the spawned sprite */

void ProjectileConfig_EnableSpriteFlag200(u8 *p)
{
    *(u16 *)(p + 0x4e) = 1;
}
