// COMPILER: old_agbcc
#include "global.h"
/* ProjectileConfig_EnableSpriteFlag200: Enables the configuration option that adds flag 0x200 to the spawned sprite */
#define ProjectileConfig_EnableSpriteFlag200 sub_08202F38

void sub_08202F38(u8 *p)
{
    *(u16 *)(p + 0x4e) = 1;
}
