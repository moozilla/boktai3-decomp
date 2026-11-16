#define SpriteGroup_SetSharedPosition Axe_SetAuxiliarySpriteRenderState
#include "global.h"
#include "scene_actor_reset.h"
struct ActorGroupMotionStateBeginExtendedMotion {
    u8 unknown0[0x8A8];
    struct ContactPosition position;
    u8 unknown890[0x59c];
    u32 flags;
    u8 unknownE50[0x6C];
    u16 timer;
    u8 unknownEBE[6];
    u16 duration;
    u8 unknownEC6[10];
    u8 active;
};
void Axe_UpdateTrailFadeIn(void);
void ActorPhase_UpdateSpriteCollapse(void);
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
void ActorGroup_BeginExtendedMotion(struct ChaseActor *actor)
{
    struct ActorGroupMotionStateBeginExtendedMotion *group = actor->base.context;
    if (TakePending(actor)) {
        BeginControl(actor, Axe_UpdateTrailFadeIn, 0x27);
        {
            u32 flag = 0x100;
            flag |= actor->base.motionFlags;
            actor->base.motionFlags = flag;
        }
        {
            u32 flag = 0x1000;
            group->flags |= flag;
        }
        group->timer = group->duration;
        SpriteGroup_SetSharedPosition(actor, &group->position, 0x13D);
        group->active = 1;
        Sound_PlaySong(0x16F);
    }
    if (actor->base.stepDone) {
        BeginControl(actor, ActorPhase_UpdateSpriteCollapse, 0x23);
        SetPhase(actor, 0, 2, 7);
    } else {
        actor->base.counter2AC++;
    }
}
