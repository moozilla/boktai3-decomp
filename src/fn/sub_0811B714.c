#include "global.h"

extern u32 gUnk_02000200;
void Collider_InitManagerBuckets(u32);

s32 Collider_InitManager(u32 a)
{
    Collider_InitManagerBuckets(a);
    gUnk_02000200 = a;
    return 0;
}
