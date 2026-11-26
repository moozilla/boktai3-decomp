#include "global.h"

s32 Player_GetLinkedSmoothedDirection(u8 *);
s32 Player_ComputeLinkedActionDelay(u8 *);
s32 Player_GetMovementAnimationRate(s32);
void LinkPlayer_SetScaledDirectionalVelocity(u8 *, s32);
void Player_PlayActionCueSound(u8 *, u32);
void Player_UpdateActionFacingSide(u8 *);
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
    p[0x3A4] = Player_GetLinkedSmoothedDirection(p);
    if (p[0x48C] == 0) {
        v = Player_ComputeLinkedActionDelay(p);
        Player_GetMovementAnimationRate(v);
        if (p != 0 && IsScaled(p) != 0)
            v = (p[0xC0] * v) >> 6;
        LinkPlayer_SetScaledDirectionalVelocity(p, v);
    }
    Player_PlayActionCueSound(p, 0);
    Player_UpdateActionFacingSide(p);
    Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p[0x418], 1), 0x40);
}
