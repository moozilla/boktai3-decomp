#include "global.h"

s32 Player_OnWeaponHitApplyFrost(u8 *p)
{
    p[0xAC9] = 4;
    return 8;
}
