#include "global.h"

s32 Player_OnWeaponHitApplyCloud(u8 *p)
{
    p[0xAC9] = 5;
    return 16;
}
