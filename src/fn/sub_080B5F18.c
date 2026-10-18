#include "global.h"
void Skeleton_UpdateActionCooldown(u8 *p)
{
    u8 *q = *(u8 **)(p + 0x3d0);
    u16 *h = (u16 *)(q + 0x704);
    if (*h != 0)
        *h -= 1;
}
