#include "global.h"
void CharacterModel_SetAnimationFlags(u8 *p, u32 m)
{
    u32 *q = (u32 *)(p + 8);
    s32 i = 4;
    do {
        *q |= m;
        q += 0x18;
    } while (--i >= 0);
}
