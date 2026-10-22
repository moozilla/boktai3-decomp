#include "global.h"
extern u8 *gUnk_02000580;
static inline s32 Sub256(s32 x)
{
    return x - 256;
}
s32 sub_081FB7D8(u32 x)
{
    s32 v = gUnk_02000580[0x4f4];
    s32 r;
    if (v >= (s32)x)
        r = v - x;
    else
        r = v - Sub256(x);
    return r;
}
