#include "global.h"

s32 Projectile_Spawn(u8 *);

s32 Projectile_CreateForOwner(u8 *p, s32 a, u32 n)
{
    if (n > 3) {
        return -1;
    }
    *(u16 *)(p + 0x38) = n;
    return Projectile_Spawn(p);
}
