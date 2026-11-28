#include "global.h"
#include "player/player.h"

void sub_0823A060(struct Player *);
void LinkPlayer_AdvanceActionTick(struct Player *);
s32 sub_08239F74(struct Player *, s32);
s32 sub_08239EB4(struct Player *, s32);
s32 ActorEffect_SelectFlashingValue(struct Player *, s32);
void sub_08239A20(struct Player *, s32);
void SpriteEffect_UpdateBriefAnimation(struct Player *);
void Player_UpdateOrbitingAuraEffect(struct Player *);
void sub_08238FE0(struct Player *);
void Player_UpdateConvergingParticles(struct Player *);
void Callback_InvokeActor(void *, void *);

// confidence=low: per-frame link player update; refreshes effects and the world position.
void sub_0823A2E4(struct Player *p)
{
    sub_0823A060(p);
    LinkPlayer_AdvanceActionTick(p);
    sub_08239A20(p, ActorEffect_SelectFlashingValue(p, sub_08239EB4(p, sub_08239F74(p, 0))));
    SpriteEffect_UpdateBriefAnimation(p);
    Player_UpdateOrbitingAuraEffect(p);
    sub_08238FE0(p);
    Player_UpdateConvergingParticles(p);
    {
        void **callback = &p->directionalEffectCallback;

        Callback_InvokeActor(p->directionalEffect, *callback);
    }
    p->effectPosition[0] = p->position.x + p->resetValue6F8;
    p->effectPosition[1] = p->position.y + p->resetValue6FA;
    p->effectPosition[2] = p->position.z + p->resetValue6FC;
}
