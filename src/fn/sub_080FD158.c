#define SpriteGroup_SetSharedPosition Axe_SetAuxiliarySpriteRenderState
#define Axe_UpdateFadeOutPhase        sub_080FD158
#include "global.h"
#include "scene_actor_reset.h"
struct ActorGroupMotionState {
    u8 unknown0[0x888];
    struct ContactPosition position;
    u8 unknown890[0x5BC];
    u32 flags;
};
void Axe_UpdateFadeOut(void);
void ActorPhase_UpdateSpriteCollapse(void);
void Axe_EmitMotionTrail(struct SceneActorRegistryResetView *);
void SpriteGroup_SetSharedPosition(struct SceneActorRegistryResetView *, void *, u32);
static inline u8 TakePending(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void BeginControl(struct SceneActorRegistryResetView *actor, void *control, u8 kind)
{
    u32 zero;
    actor->field2B2 = 1;
    zero = 0;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = zero;
}
static inline void SetPhase(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
void Axe_UpdateFadeOutPhase(struct SceneActorRegistryResetView *actor)
{
    struct ActorGroupMotionState *group = actor->context;
    if (TakePending(actor)) {
        BeginControl(actor, Axe_UpdateFadeOut, 0x22);
        {
            u32 flag = 0x100;
            flag |= actor->flags2D4;
            actor->flags2D4 = flag;
        }
        SpriteGroup_SetSharedPosition(actor, &group->position, 0x13C);
        {
            u32 flag = 0x400;
            group->flags |= flag;
        }
    }
    Axe_EmitMotionTrail(actor);
    if (actor->field2B0) {
        BeginControl(actor, ActorPhase_UpdateSpriteCollapse, 0x23);
        SetPhase(actor, 0, 2, 1);
    } else {
        actor->counter2AC++;
    }
}
