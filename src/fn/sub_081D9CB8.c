#include "global.h"

u32 Projectile_UpdateAnimationTail(u8 *p)
{
    if (*(s16 *)(p + 0x68) <= 0) {
        return 2;
    }
    *(u32 *)(p + 0x8C) |= 1;
    return 0;
}
