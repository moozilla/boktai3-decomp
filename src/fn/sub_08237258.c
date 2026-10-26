#include "global.h"
#include "ram.h"
void Effect_Remove(void *);
u32 RisingPuffPool_DestroyTask(u8 *p)
{
    u8 *e = p + 0x18;
    s32 i = 0x7;
    do {
        Effect_Remove(e);
        e += 0x30;
        i--;
    } while (i >= 0);
    {
        u32 z = 0;
        gRisingSpritePool = z;
    }
    return 0;
}
