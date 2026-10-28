#include "global.h"
#include "ram.h"
u32 HomingProjectile_GetActiveCount(void)
{
    return *(u32 *)(((u8 *)gHomingManager) + 0x43c);
}
