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
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = code;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
static inline void ClearFlags(u32 *flags, u32 mask)
{
    *flags &= mask;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    u32 b = behavior;
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = b;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
// confidence=low: category-seven angle/range gate chooses close or ranged behavior
u32 Skeleton_TryBeginArmedAttack(struct SceneActorRegistryResetView *actor,
                                 struct SkeletonAttackContext *context, s32 angle)
{
    if (actor->base.tracking.distance < context->retreatRange) {
        if (actor->base.health < context->guardHealth || TestBits(context, 0x80)) {
            if (angle <= 24) {
                if (actor->base.tracking.distance < context->nearRange && context->nearTimer <= 0) {
                    SetControl(actor, SceneActorControl_OnIdle, 0x10);
                    SetState(actor, 0, 2, 0);
                    context->attempts = 0;
                    return 1;
                }
                if (angle <= 24 && context->rangedTimer <= 0) {
                    SetControl(actor, SceneActorControl_OnIdle, 0x10);
                    SetState(actor, 0, 2, 3);
                    ClearFlags(&context->flags, ~0x80);
                    context->attempts = 0;
                    return 1;
                }
            }
        }
        if (angle <= 24 && context->attempts) {
            SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SetState(actor, 0, 2, 3);
            ClearFlags(&context->flags, ~0x80);
            context->attempts = 0;
            return 1;
        }
        Skeleton_SelectDistanceReaction(actor);
        return 1;
    }
    if (angle <= 20) {
        if ((actor->base.tracking.distance < context->rangedRange && context->rangedTimer <= 0) ||
            context->attempts) {
            SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SetState(actor, 0, 2, 3);
            context->attempts = 0;
            return 1;
        }
    }
    return 0;
}
