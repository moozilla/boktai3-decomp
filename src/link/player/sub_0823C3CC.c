#include "global.h"
void Player_SetTetherMode(u8 *, s32, s32);
s32 Player_SelectHistoryDirectionB(u8 *);
void sub_08238238(u8 *);
s32 ActorVariant_GetTableValue(u32, s32);
void Player_SetAnimationIndex(u8 *, s32, s32);
void Player_EnterIdleAnimation(u8 *p)
{
    s32 value;
    if (p[0x457]) {
        Player_SetTetherMode(p, 0, 0);
        p[0x3a4] = Player_SelectHistoryDirectionB(p);
        sub_08238238(p);
    }
    *(u32 *)(p + 0x20) &= ~1;
    value = ActorVariant_GetTableValue(p[0x418], 0);
    Player_SetAnimationIndex(p, value, 0x40);
}
