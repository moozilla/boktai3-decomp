#define ActorGroup_BeginMotionPhase   sub_080FD334
#define SpriteGroup_SetSharedPosition Axe_SetAuxiliarySpriteRenderState
#include "global.h"
#include "scene_actor_reset.h"
struct ActorGroupMotionState {
    u8 unknown0[0x888];
    struct ContactPosition position;
    u8 unknown890[0x5bc];
    u32 flags;
    u8 unknownE50[0x68];
    u16 timer;
    u8 unknownEBA[6];
    u16 duration;
};
void sub_080F8B38(void);
void sub_080F82E8(void);
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
void sub_080FD334(struct SceneActorRegistryResetView *actor)
{
    struct ActorGroupMotionState *group = actor->context;
    if (TakePending(actor)) {
        BeginControl(actor, sub_080F8B38, 0x31);
        {
            u32 flag = 0x100;
            flag |= actor->flags2D4;
            actor->flags2D4 = flag;
        }
        {
            u32 flag = 0x200;
            group->flags |= flag;
        }
        SpriteGroup_SetSharedPosition(actor, &group->position, 0x13c);
        group->timer = group->duration;
    }
    if (actor->field2B0) {
        BeginControl(actor, sub_080F82E8, 0x23);
        SetPhase(actor, 0, 2, 3);
    } else {
        actor->counter2AC++;
    }
}
