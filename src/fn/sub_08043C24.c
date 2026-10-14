#include "global.h"
#define ContactActorPool_Destroy sub_08043C24

void ContactActor_DestroySlot(u8 *, u8 *, s32);

u32 sub_08043C24(u8 *p)
{
    u8 *q = p + 0x3C;
    s32 i = 0;
    do {
        ContactActor_DestroySlot(p, q, i);
        i++;
        q += 0x16c;
    } while (i <= 15);
    return 0;
}
