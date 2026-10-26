#include "global.h"
#include "ram.h"

void Collider_InitManagerBuckets(u32);

s32 Collider_InitManager(u32 a)
{
    Collider_InitManagerBuckets(a);
    gSpatialIndex = a;
    return 0;
}
