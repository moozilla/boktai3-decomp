#include "global.h"

s32 Player_OnWeaponHitApplySol(u8 *p)
{
    p[0xAC9] = 1;
    return 1;
}
