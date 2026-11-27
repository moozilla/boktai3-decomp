#include "global.h"

void CharacterModel_SetAnimationFlags(u8 *, u32);

void LinkScorePanel_HideSprites(u8 *p)
{
    s32 i;

    for (i = 0; i < 18; i++)
        *(u32 *)(p + 0x2C + i * 0x60) |= 1;
    CharacterModel_SetAnimationFlags(p + 0xD8C, 1);
}
