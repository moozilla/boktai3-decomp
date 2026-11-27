#include "global.h"
#include "engine/ram.h"
/* Effect pool family names and field interpretations are tentative. */
void *HresvelgrEffectPool_Create(s32, s32, s32);
void HresvelgrEffectPool_AllocateEntry(u32, u32, u32, s32);
void HresvelgrEffectPool_CreateEntry(u32 a, u32 b, u32 c, s32 count)
{
    if (!gEffectPoolManager)
        gEffectPoolManager = HresvelgrEffectPool_Create(0, 0, 0);
    if (count <= 0)
        count = 1;
    HresvelgrEffectPool_AllocateEntry(a, b, c, count);
}
