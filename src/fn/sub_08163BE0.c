#include "global.h"

void Effect_Remove(u8 *);

s32 ParticlePool_OnDestroy(u8 *p)
{
    u8 *q = p + 0x74;
    s32 i = 15;
    do {
        Effect_Remove(q);
        q += 0x3c;
        i--;
    } while (i >= 0);
    return 0;
}
