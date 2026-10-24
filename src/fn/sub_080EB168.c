#include "global.h"
#include "scene_actor_control_phase.h"
void SceneActor_BeginReactionAnimation(void);
s32 SceneActorReaction_FindAdjacentTarget(struct SceneActorRegistryResetView *, s32);
static inline void SetControl(struct SceneActorRegistryResetView *a, void *cb, u32 k)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = k;
    a->target = cb;
    a->field2C8 = 0;
    {
        u32 *p = &a->flags114;
        u32 mask = ~1;
        *p &= mask;
    }
    a->word104 = 0;
}
static inline void State(struct SceneActorRegistryResetView *a, u8 state, u8 phase, u8 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
static inline void Or(u16 m, u16 *f)
{
    *f |= m;
}
// confidence=low: hides the actor and waits for an adjacent target; one starts the reaction
// animation, otherwise after 60 frames it moves on to phase 2, behavior 5.
#define SceneActorReaction_WaitHiddenForAdjacentTarget sub_080EB168
void SceneActorReaction_WaitHiddenForAdjacentTarget(struct SceneActorRegistryResetView *a)
{
    if (SceneActorControl_TakeFlag(&a->field2B1)) {
        u32 flag;
        {
            struct ContactCollider *c = &a->colliderA;
            u16 v = 4 | c->flags;
            c->flags = v & 0xfff7;
        }
        Or(0x800, &a->flags2C0);
        flag = 1;
        if (a->drawableKind == 0) {
            u32 *flags = a->drawable;
            *flags |= flag;
        } else {
            u32 *flags = (u32 *)((u8 *)a->drawable + 0x20);
            flags[2] |= flag;
        }
    }
    if (SceneActorReaction_FindAdjacentTarget(a, 1) == 0) {
        if ((s32)a->counter2AC > 60)
            State(a, 0, 2, 5);
        else
            a->counter2AC++;
    } else {
        State(a, 0, 0, 2);
        SetControl(a, (void *)SceneActor_BeginReactionAnimation, 41);
    }
}
