#define SpriteGroup_SetSharedPosition Axe_SetAuxiliarySpriteRenderState
#include "global.h"
#include "scene_actor_reset.h"
struct ActorGroupMotionState {
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
void ActorGroup_BeginExtendedMotion(struct SceneActorRegistryResetView *actor)
{
    struct ActorGroupMotionState *group = actor->context;
    if (TakePending(actor)) {
        BeginControl(actor, Axe_UpdateTrailFadeIn, 0x27);
        {
            u32 flag = 0x100;
            flag |= actor->flags2D4;
            actor->flags2D4 = flag;
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
    if (actor->field2B0) {
        BeginControl(actor, ActorPhase_UpdateSpriteCollapse, 0x23);
        SetPhase(actor, 0, 2, 7);
    } else {
        actor->counter2AC++;
    }
}
