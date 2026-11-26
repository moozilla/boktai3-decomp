#include "global.h"

s32 Player_GetSpecialAttackPower(s32, s32);

s32 Player_GetEquipmentLevelBonus(u8 *p)
{
    return Player_GetSpecialAttackPower(0, p[0] - 1);
}
