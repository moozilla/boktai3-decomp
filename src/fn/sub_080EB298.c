#include "global.h"
#include "scene_actor_control_phase.h"
void SceneActor_BeginReactionAnimation(void);
void SceneActorControl_OnIdle(void);
void sub_080E9C20(void);
s32 SceneActorReaction_FindAdjacentTarget(struct ChaseActor *, s32);
static inline void SetControl(struct ChaseActor *a, void *cb, u32 k)
{
    a->base.field2B2 = 1;
    a->base.field2B0 = 0;
    a->base.field2B5 = k;
    a->base.target = cb;
    a->base.field2C8 = 0;
    {
        u32 *p = &a->base.flags114;
        u32 mask = ~1;
        *p &= mask;
    }
    a->base.word104 = 0;
}
static inline void State(struct ChaseActor *a, u32 state)
{
    a->base.state2A8 = state;
    a->base.phase2A9 = 0;
    a->base.behavior2AA = 0;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}
static inline void IdleState(struct ChaseActor *a)
{
    a->base.state2A8 = 0;
    a->base.phase2A9 = 0;
    a->base.behavior2AA = 0;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}
// confidence=low: reacquires an adjacent target after the entry animation, or times out to idle.
void SceneActorReaction_ReacquireAdjacentTarget(struct ChaseActor *a)
{
    if (SceneActorControl_TakeFlag(&a->base.field2B1)) {
        SetControl(a, (void *)SceneActor_BeginReactionAnimation, 41);
        a->base.restart = 0;
    }
    if (a->base.restart || a->base.field2B5 == 34) {
        if (SceneActorReaction_FindAdjacentTarget(a, 1)) {
            if (!a->base.drawableKind) {
                u32 *p = a->base.drawable;
                u32 mask = ~1;
                *p &= mask;
            } else {
                u32 *p = (void *)((u8 *)a->base.drawable + 0x20);
                u32 mask = ~1;
                p[2] &= mask;
            }
            State(a, 3);
            SetControl(a, (void *)sub_080E9C20, 3);
        }
        if (a->base.field2B5 == 34 && (s32)a->base.counter2AC > 180) {
            IdleState(a);
            SetControl(a, (void *)SceneActorControl_OnIdle, 16);
            return;
        }
    }
    a->base.counter2AC++;
}
