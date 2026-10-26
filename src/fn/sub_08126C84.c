#include "global.h"
#include "ram.h"
void Sprite_Remove(u8 *);
u32 BurstSpark_DestroyPool(u8 *p)
{
    u8 *q = p + 0x38;
    u8 *r = p + 0x77;
    s32 n = 0x7f;
    u32 z;
    do {
        if (*r == 1 && q[4] != 0)
            Sprite_Remove(q);
        q += 0x40;
        r += 0x40;
        n--;
    } while (n >= 0);
    z = 0;
    gBurstSparkPool = z;
    return z;
}
