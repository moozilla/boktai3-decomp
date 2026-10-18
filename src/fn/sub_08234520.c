#include "global.h"
void Effect_Remove(u8 *);
u32 SpiralBurst_OnTaskDestroy(u8 *p)
{
    s32 i;
    u8 *q = p + 0x78;
    for (i = 7; i >= 0; i--) {
        Effect_Remove(q);
        q += 0x30;
    }
    return 0;
}
