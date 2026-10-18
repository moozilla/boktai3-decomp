#include "global.h"
/* Effect pool family names and field interpretations are tentative. */
#define gEffectPool gUnk_0200023C
extern void *gUnk_0200023C;
void *EffectPool_Create(s32, s32, s32);
void EffectPool_AllocateEntry(u32, u32, u32, s32);
void EffectPool_CreateEntry(u32 a, u32 b, u32 c, s32 count)
{
    if (!gEffectPool)
        gEffectPool = EffectPool_Create(0, 0, 0);
    if (count <= 0)
        count = 1;
    EffectPool_AllocateEntry(a, b, c, count);
}
