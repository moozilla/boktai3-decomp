#include "global.h"

s32 Player_CanUseAction(u8 *, s32);
void sub_0823B5D8(u8 *);
void ActorMotion_ApplyHeadingDrift(u8 *);
void Player_ClearMovementLock(u8 *);

void sub_0823B728(u8 *p, s32 action)
{
    if ((**(u16 **)(p + 0x348) & 0x100) && Player_CanUseAction(p, action)) {
        u32 kind = p[0x418];
        if ((kind <= 1 || kind == 6) && p[0x457] == 4)
            sub_0823B5D8(p);
        else
            ActorMotion_ApplyHeadingDrift(p);
    } else if (*(s16 *)(p + 0x4B6) != 0) {
        Player_ClearMovementLock(p);
    }
    p[0x4B9] = 1;
}
