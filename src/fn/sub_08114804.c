#define EffectActor_SetUpdate NativeEffect_SetState
#include "global.h"
struct EffectMotionActor {
    u8 pad0[0x8c]; u8 animation[0x10]; void *animationData;
    s32 x; u8 padA4[0xc]; s32 velocity; u8 padB4[0x10]; s32 targetVelocity;
    u8 padC8[8]; s32 direction;
};
struct EffectMotionConfig { u8 pad0[6]; u16 value; };
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void NativeEffect_SetState(void *, void (*)(void));
void NativeEffect_InitColliderWithDepth(void *, void (*)(void), u32, u32);
void NativeEffect_OnColliderImpact(void);
void sub_08113920(void);
void EffectActor_BeginHorizontalMotion(struct EffectMotionActor *actor, struct EffectMotionConfig *config, s32 reverse)
{
    actor->velocity += 0x2000;
    actor->targetVelocity = actor->velocity;
    NativeEffect_InitColliderWithDepth(actor, NativeEffect_OnColliderImpact, config->value, 10);
    NativeEffect_SetState(actor, sub_08113920);
    if (reverse) {
        actor->x -= 0x10000;
        Anim_SetSequence(actor->animation, actor->animationData, 8, 0, 1);
        { s32 velocity = actor->velocity; actor->velocity = actor->targetVelocity - velocity; }
        actor->direction = -1;
    } else {
        actor->x += 0x8000;
        Anim_SetSequence(actor->animation, actor->animationData, 8, 0, 0);
        actor->direction = 1;
    }
}
