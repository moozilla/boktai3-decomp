#include "global.h"

void ProjectileTask_RemoveEntry(s32);

void Player_OnChargedAttackExit(u8 *p)
{
    s32 v = *(s32 *)(p + 0xC74);

    if (v >= 0) {
        ProjectileTask_RemoveEntry(v);
    }
}
