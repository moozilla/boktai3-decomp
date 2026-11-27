#include "global.h"

s32 Player_CalculateTotalAttackPower(u8 *, s32);

s32 Player_GetAttackPower(u8 *p)
{
    return Player_CalculateTotalAttackPower(p, 0);
}
