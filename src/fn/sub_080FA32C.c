#include "scene_actor_control_phase.h"
u8 Axe_PickAttack(void *);
void ActorPhase_UpdateSpriteCollapse(void);
extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline void Polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void SetControl(struct ChaseActor *actor, void *callback, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}
static inline void SetState(struct ChaseActor *a, u8 s, u8 p, u8 b)
{
    a->base.state2A8 = s;
    a->base.phase2A9 = p;
    a->base.behavior2AA = b;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}
// confidence=low: speed and phase duration depend on remaining health.
void Axe_UpdateHealthPacedMotion(struct ChaseActor *actor)
{
    u8 *context = actor->base.context;
    s32 speed;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        SetControl(actor, actor->base.onStanding, 0);
    if ((u8)Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x270)) && Axe_PickAttack(actor))
        return;
    if (actor->base.health <= *(s32 *)(context + 0xE48)) {
        speed = 8;
        if ((s32)actor->base.counter2AC > 400) {
            SetControl(actor, ActorPhase_UpdateSpriteCollapse, 35);
            SetState(actor, 0, 0, 4);
            return;
        }
    } else if (actor->base.health <= *(u16 *)(context + 0xEB4)) {
        speed = 6;
        if ((s32)actor->base.counter2AC > 500) {
            SetControl(actor, ActorPhase_UpdateSpriteCollapse, 35);
            SetState(actor, 0, 0, 4);
            return;
        }
    } else {
        speed = 4;
        if ((s32)actor->base.counter2AC > 600) {
            SetState(actor, 0, 0, 4);
            return;
        }
    }
    Polar(&actor->base.body.motion, actor->base.tracking.heading, speed);
    actor->base.counter2AC++;
}
