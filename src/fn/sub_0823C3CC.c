#include "global.h"
void Player_SetTetherMode(u8 *, s32, s32);
s32 sub_0823C368(u8 *);
void sub_08238238(u8 *);
s32 ActorVariant_GetTableValue(u32, s32);
void sub_08238280(u8 *, s32, s32);
void Player_EnterIdleAnimation(u8 *p)
{
    s32 value;
    if (p[0x457]) {
        Player_SetTetherMode(p, 0, 0);
        p[0x3a4] = sub_0823C368(p);
        sub_08238238(p);
    }
    *(u32 *)(p + 0x20) &= ~1;
    value = ActorVariant_GetTableValue(p[0x418], 0);
    sub_08238280(p, value, 0x40);
}
