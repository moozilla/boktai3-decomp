#include "global.h"

s32 sub_0823C4B4(u8 *);
s32 sub_0823C424(u8 *);
s32 Player_GetMovementAnimationRate(void);
void Player_SetDirectionalVelocity_08238300(u8 *, s32);
void Player_PlayActionCueSound(u8 *, u32);
void sub_08238238(u8 *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(u8 *, s32, s32);

static inline s32 IsScaled(u8 *p)
{
    u32 a = *(u16 *)(p + 0x538);
    s32 r = 0;
    if (a != 0) {
        if (*(u16 *)(p + 0x534) == 5)
            r = 1;
    }
    return r;
}

void sub_0823C6BC(u8 *p)
{
    s32 v;
    p[0x457] = 1;
    *(u32 *)(p + 0x20) &= ~1;
    p[0x3A4] = sub_0823C4B4(p);
    if (p[0x48C] == 0) {
        v = sub_0823C424(p);
        Player_GetMovementAnimationRate();
        if (p != 0 && IsScaled(p) != 0)
            v = (p[0xC0] * v) >> 6;
        Player_SetDirectionalVelocity_08238300(p, v);
    }
    Player_PlayActionCueSound(p, 0);
    sub_08238238(p);
    Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p[0x418], 1), 0x40);
}
