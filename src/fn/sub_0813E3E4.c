#include "global.h"

s32 Player_OnWeaponHitApplyFlame(u8 *p)
{
    p[0xAC9] = 3;
    return 4;
}
