#include "global.h"
void CharacterModel_SetAnimationFlags(void *, s32);
void StageResults_HideSprites(u8 *s)
{
    s32 i;
    for (i = 0; i < 23; i++)
        *(u32 *)(s + 0x2c + i * 0x60) |= 1;
    CharacterModel_SetAnimationFlags(s + 0xF60, 1);
}
