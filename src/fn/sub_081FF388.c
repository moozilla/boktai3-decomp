#include "global.h"
extern u8 *gHomingManager;
u32 HomingProjectile_GetActiveCount(void)
{
    return *(u32 *)(gHomingManager + 0x43c);
}
