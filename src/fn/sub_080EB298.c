#include "global.h"
#include "scene_actor_control_phase.h"
void SceneActor_BeginReactionAnimation(void);
void SceneActorControl_OnIdle(void);
void sub_080E9C20(void);
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
static inline void State(struct SceneActorRegistryResetView *a, u32 state)
{
    a->state2A8 = state;
    a->phase2A9 = 0;
    a->behavior2AA = 0;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
static inline void IdleState(struct SceneActorRegistryResetView *a)
{
    a->state2A8 = 0;
    a->phase2A9 = 0;
    a->behavior2AA = 0;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
// confidence=low: reacquires an adjacent target after the entry animation, or times out to idle.
void SceneActorReaction_ReacquireAdjacentTarget(struct SceneActorRegistryResetView *a)
{
    if (SceneActorControl_TakeFlag(&a->field2B1)) {
        SetControl(a, (void *)SceneActor_BeginReactionAnimation, 41);
        a->field2B3 = 0;
    }
    if (a->field2B3 || a->field2B5 == 34) {
        if (SceneActorReaction_FindAdjacentTarget(a, 1)) {
            if (!a->drawableKind) {
                u32 *p = a->drawable;
                u32 mask = ~1;
                *p &= mask;
            } else {
                u32 *p = (void *)((u8 *)a->drawable + 0x20);
                u32 mask = ~1;
                p[2] &= mask;
            }
            State(a, 3);
            SetControl(a, (void *)sub_080E9C20, 3);
        }
        if (a->field2B5 == 34 && (s32)a->counter2AC > 180) {
            IdleState(a);
            SetControl(a, (void *)SceneActorControl_OnIdle, 16);
            return;
        }
    }
    a->counter2AC++;
}
