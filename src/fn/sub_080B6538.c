#include "global.h"
#include "scene_actor_timed_behavior.h"
u8 SceneActorReaction_IsRangeReady(struct SceneActorRegistryResetView *);
void Skeleton_SelectDistanceReaction(struct SceneActorRegistryResetView *);
void SceneActorControl_OnIdle(void);
static inline u8 IsFlag512Set(struct SceneActorTimedBehaviorContext *context)
{
    u32 mask = 0x200;
    if (context->flags6DC & mask)
        return 1;
    return 0;
}
u32 SceneActorControl_TryBeginNearbyBehavior16(struct SceneActorRegistryResetView *s,
                                               struct SceneActorTimedBehaviorContext *context,
                                               s32 angle)
{
    if (!SceneActorReaction_IsRangeReady(s)) {
        if (IsFlag512Set(context))
            return 1;
        Skeleton_SelectDistanceReaction(s);
        return 1;
    }
    if (angle <= 24 && s->base.tracking.distance < context->nearRange6E0) {
        void (*fn)(void) = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u32 zero;
        u8 one;
        u8 zero8;
        u32 *flags;
        u8 *taken;
        u32 two;
        taken = &s->base.field2B2;
        zero = 0;
        one = 1;
        *taken = one;
        s->base.field2B0 = zero;
        s->base.field2B5 = code;
        s->base.target = fn;
        zero8 = 0;
        s->base.field2C8 = zero;
        flags = &s->base.flags114;
        {
            u32 mask = -2;
            *flags &= mask;
        }
        s->base.word104 = zero;
        two = 2;
        s->base.state2A8 = zero8;
        s->base.phase2A9 = two;
        s->base.behavior2AA = zero8;
        s->base.counter2AC = zero;
        s->base.field2B1 = one;
        context->attempts703 = zero8;
        return 1;
    }
    return 0;
}
