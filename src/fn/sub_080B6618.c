#include "global.h"
#include "scene_actor_reset.h"
struct SkeletonAttackContext {
    u8 unknown0[0x6DC];
    u32 flags;
    u32 nearRange, rangedRange, guardRange;
    u8 unknown6EC[4];
    s16 nearTimer, rangedTimer, guardTimer;
    u8 unknown6F6[0xD];
    u8 attempts;
    u8 unknown704[4];
    u32 retreatRange, pursuitRange;
    u8 unknown710[8];
    s32 guardHealth;
};
void SceneActorControl_OnIdle(void);
void Skeleton_SelectDistanceReaction(struct SceneActorRegistryResetView *);
static inline u8 TestBits(struct SkeletonAttackContext *context, u32 mask)
{
    if (context->flags & mask)
        return 1;
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    void *callback = control;
    u32 code = kind;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = code;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void ClearFlags(u32 *flags, u32 mask)
{
    *flags &= mask;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    u32 b = behavior;
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
u8 SceneActorReaction_IsRangeReady(struct SceneActorRegistryResetView *);
// confidence=low: category-six attack gate selects close, ranged or health-gated guard behavior
u32 Skeleton_TryBeginGuardedAttack(struct SceneActorRegistryResetView *actor,
                                   struct SkeletonAttackContext *context, s32 angle)
{
    if (!SceneActorReaction_IsRangeReady(actor)) {
        if (TestBits(context, 0x200))
            return 1;
        Skeleton_SelectDistanceReaction(actor);
        return 1;
    }
    if (actor->tracking.distance < context->guardRange && context->guardTimer <= 0) {
        if (angle <= 12 && actor->health <= context->guardHealth) {
            SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SetState(actor, 0, 2, 2);
            context->attempts = 0;
            return 1;
        }
        if (angle <= 24 && context->nearTimer <= 0) {
            SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SetState(actor, 0, 2, 0);
            context->attempts = 0;
            return 1;
        }
    } else {
        if (angle <= 24 && actor->tracking.distance < context->nearRange && context->nearTimer <= 0) {
            SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SetState(actor, 0, 2, 0);
            context->attempts = 0;
            return 1;
        }
        if (angle <= 12 && actor->tracking.distance < context->rangedRange && context->rangedTimer <= 0) {
            SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SetState(actor, 0, 2, 1);
            context->attempts = 0;
            return 1;
        }
    }
    return 0;
}
