#include "global.h"

s32 Player_OnWeaponHitApplyEarth(u8 *p)
{
    p[0xAC9] = 6;
    return 32;
}
