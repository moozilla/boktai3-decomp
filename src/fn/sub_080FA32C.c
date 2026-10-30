#include "scene_actor_control_phase.h"
u8 Axe_PickAttack(void *);
void ActorPhase_UpdateSpriteCollapse(void);
extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 x)
{
    s32 r;
    if (x >= 0)
        r = x >> 12;
    else
        r = -(-x >> 12);
    return r;
}
static inline void Polar(struct ContactPosition *v, s32 angle, s32 speed)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * speed);
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
// confidence=low: speed and phase duration depend on remaining health.
void Axe_UpdateHealthPacedMotion(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    s32 speed;
    if (SceneActorControl_TakeFlag(&actor->field2B1))
        SetControl(actor, actor->savedTarget, 0);
    if ((u8)Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x270)) && Axe_PickAttack(actor))
        return;
    if (actor->health <= *(s32 *)(context + 0xE48)) {
        speed = 8;
        if ((s32)actor->counter2AC > 400) {
            SetControl(actor, ActorPhase_UpdateSpriteCollapse, 35);
            SetState(actor, 0, 0, 4);
            return;
        }
    } else if (actor->health <= *(u16 *)(context + 0xEB4)) {
        speed = 6;
        if ((s32)actor->counter2AC > 500) {
            SetControl(actor, ActorPhase_UpdateSpriteCollapse, 35);
            SetState(actor, 0, 0, 4);
            return;
        }
    } else {
        speed = 4;
        if ((s32)actor->counter2AC > 600) {
            SetState(actor, 0, 0, 4);
            return;
        }
    }
    Polar(&actor->body.motion, actor->tracking.heading, speed);
    actor->counter2AC++;
}
