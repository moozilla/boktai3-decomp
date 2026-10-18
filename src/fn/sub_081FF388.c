#include "global.h"
extern u8 *gUnk_020005F4;
u32 HomingProjectile_GetActiveCount(void)
{
    return *(u32 *)(gUnk_020005F4 + 0x43c);
}
