#include "global.h"

extern u32 gUnk_020001C8;
void Registry30AC_RemoveAnimations(u8 *);
void CharacterModel_RemoveAnimations(u8 *);
s32 Registry30AC_Destroy(u8 *p)
{
    s32 i;
    u32 z;
    Registry30AC_RemoveAnimations(p);
    p += 0x940;
    i = 1;
    do {
        CharacterModel_RemoveAnimations(p);
        p += 0x850;
        i--;
    } while (i >= 0);
    z = 0;
    gUnk_020001C8 = z;
    return 1;
}