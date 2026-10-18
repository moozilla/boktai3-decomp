#include "global.h"
void CharacterModel_ClearAnimationFlags(u8 *p, u32 m)
{
    u32 *q;
    s32 i;
    m = ~m;
    q = (u32 *)(p + 8);
    i = 4;
    do {
        *q &= m;
        q += 0x18;
    } while (--i >= 0);
}
