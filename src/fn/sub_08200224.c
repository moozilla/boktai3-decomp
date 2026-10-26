#include "global.h"
#include "ram.h"
u32 PlayerClone_GetActiveCount(void)
{
    return *(u32 *)(gDirectionalSpritePool + 0x9e8);
}
