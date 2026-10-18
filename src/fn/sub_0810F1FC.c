#include "global.h"
void CharacterModel_SetAnimationFlags(void *, s32);
void LinkLobbyPanel_HideSprites(u8 *s)
{
    s32 i;
    u8 *p;
    u32 m = 1;
    u32 *q = (u32 *)(s + 0x2c);
    i = 8;
    do {
        *q |= m;
        q = (u32 *)((u8 *)q + 0x60);
    } while (--i >= 0);
    p = s + 0xa30;
    i = 1;
    do {
        CharacterModel_SetAnimationFlags(p, 1);
        p += 0x850;
    } while (--i >= 0);
}
