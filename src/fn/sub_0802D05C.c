#include "global.h"
/* Actor_SetAnimationEntry: Sets a drawable entry with zero direction and flip parameters */
#define Actor_SetAnimationEntry sub_0802D05C

s32 DisplayAnimation_SetEntry(void *, u32, u32, u32, u32, u32, u32);

s32 sub_0802D05C(u8 *p, u32 a, u32 b, u32 unused, u32 c, u32 d)
{
    return DisplayAnimation_SetEntry(p + 0x184, a, 0, 0, b, c, d);
}
