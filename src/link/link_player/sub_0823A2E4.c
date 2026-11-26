#include "global.h"
void sub_0823A060(u8 *);
void LinkPlayer_AdvanceActionTick(u8 *);
s32 sub_08239F74(u8 *, s32);
s32 sub_08239EB4(u8 *, s32);
s32 ActorEffect_SelectFlashingValue(u8 *, s32);
void sub_08239A20(u8 *, s32);
void sub_082386DC(u8 *);
void Player_UpdateOrbitingAuraEffect(u8 *);
void sub_08238FE0(u8 *);
void Player_UpdateConvergingParticles(u8 *);
void Callback_InvokeActor(u8 *, u32);
struct LinkPos {
    u8 pad0[0x30];
    u16 pos[3];
    u8 pad36[0x6f0 - 0x36];
    u16 out[4];
    u16 base[3];
};
// confidence=low: per-frame link player update; refreshes effects and the world position.
void sub_0823A2E4(u8 *p)
{
    sub_0823A060(p);
    LinkPlayer_AdvanceActionTick(p);
    sub_08239A20(p, ActorEffect_SelectFlashingValue(p, sub_08239EB4(p, sub_08239F74(p, 0))));
    sub_082386DC(p);
    Player_UpdateOrbitingAuraEffect(p);
    sub_08238FE0(p);
    Player_UpdateConvergingParticles(p);
    {
        u32 *source = (u32 *)(p + 0x864);
        Callback_InvokeActor(p + 0x7fc, *source);
    }
    {
        struct LinkPos *q = (struct LinkPos *)p;
        q->out[0] = q->pos[0] + q->base[0];
        q->out[1] = q->pos[1] + q->base[1];
        q->out[2] = q->pos[2] + q->base[2];
    }
}
