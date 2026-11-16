#define SpriteGroup_SetSharedPosition Axe_SetAuxiliarySpriteRenderState
#include "global.h"
#include "axe.h"
#include "scene_actor_reset.h"
void Axe_UpdateFadeOut(void);
void ActorPhase_UpdateSpriteCollapse(void);
void Axe_EmitMotionTrail(struct ChaseActor *);
void SpriteGroup_SetSharedPosition(struct ChaseActor *, void *, u32);
static inline u8 TakePending(struct ChaseActor *actor)
{
    if (actor->base.field2B1) {
        actor->base.field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void BeginControl(struct ChaseActor *actor, void *control, u8 kind)
{
    u32 zero;
    actor->base.field2B2 = 1;
    zero = 0;
    actor->base.stepDone = zero;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = zero;
}
static inline void SetPhase(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
void Axe_UpdateFadeOutPhase(struct ChaseActor *actor)
{
    struct Axe *group = actor->base.context;
    if (TakePending(actor)) {
        BeginControl(actor, Axe_UpdateFadeOut, 0x22);
        {
            u32 flag = 0x100;
            flag |= actor->base.motionFlags;
            actor->base.motionFlags = flag;
        }
        SpriteGroup_SetSharedPosition(actor, &group->position, 0x13C);
        {
            u32 flag = 0x400;
            group->flagsE4C |= flag;
        }
    }
    Axe_EmitMotionTrail(actor);
    if (actor->base.stepDone) {
        BeginControl(actor, ActorPhase_UpdateSpriteCollapse, 0x23);
        SetPhase(actor, 0, 2, 1);
    } else {
        actor->base.counter2AC++;
    }
}
