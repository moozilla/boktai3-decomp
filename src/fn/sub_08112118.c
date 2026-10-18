#include "global.h"
void CharacterModel_SetAnimationFlags(void *, s32);
void StageResults_HideSprites(u8 *s)
{
    s32 i;
    u32 m = 1;
    u32 *q = (u32 *)(s + 0x2c);
    i = 0x16;
    do {
        *q |= m;
        q = (u32 *)((u8 *)q + 0x60);
    } while (--i >= 0);
    CharacterModel_SetAnimationFlags(s + 0xF60, 1);
}
