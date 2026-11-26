// COMPILER: old_agbcc
#include "global.h"
/* ProjectileConfig_EnableOption58: Sets the optional halfword at offset 0x58 to one; its specific effect remains tentative */
#define ProjectileConfig_EnableOption58 sub_08202F40

void sub_08202F40(u8 *p)
{
    *(u16 *)(p + 0x58) = 1;
}
