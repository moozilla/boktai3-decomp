#include "global.h"
/* Effect pool family names and field interpretations are tentative. */
#define EffectPool_CreateEntry   sub_081A3FDC
#define EffectPool_Create        sub_081A3F84
#define EffectPool_AllocateEntry sub_081A388C
#define gEffectPool              gUnk_0200023C
extern void *gUnk_0200023C;
void *sub_081A3F84(s32, s32, s32);
void sub_081A388C(u32, u32, u32, s32);
void sub_081A3FDC(u32 a, u32 b, u32 c, s32 count)
{
    if (!gEffectPool)
        gEffectPool = EffectPool_Create(0, 0, 0);
    if (count <= 0)
        count = 1;
    EffectPool_AllocateEntry(a, b, c, count);
}
