#include "global.h"

void CharacterModel_SetAnimationFlags(u8 *, u32);

void LinkScorePanel_HideSprites(u8 *p)
{
    u32 flag = 1;
    u8 *q = p + 0x2C;
    s32 i;

    for (i = 0x11; i >= 0; i--) {
        *(u32 *)q |= flag;
        q += 0x60;
    }
    CharacterModel_SetAnimationFlags(p + 0xD8C, 1);
}
