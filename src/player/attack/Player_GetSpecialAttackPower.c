#include "global.h"

extern u8 *gGameState;
extern u8 gUnk_08E60F4C[];

s32 Player_GetSpecialAttackPower(s32 unused, s32 idx)
{
    u8 *t = gUnk_08E60F4C;
    u8 *e = t + (idx << 2);
    return *(s16 *)(gGameState + 0x40) + e[1];
}
