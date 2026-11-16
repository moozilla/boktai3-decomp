#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
/* Skeleton_UpdateTargetTransition: skeleton variant of the target-motion transition */
struct SkeletonTargetContext {
    u8 unknown0[0x6DC];
    u32 flags;
    u8 unknown6E0[0x2C];
    u32 timeLimit;
};
void Skeleton_SelectDistanceReaction(struct SceneActorRegistryResetView *);
static inline u8 TakePrepared(struct SceneActorRegistryResetView *a)
{
    if (a->base.field2B2) {
        a->base.field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline u8 IsActive(u32 flags)
{
    if (flags)
        return 1;
    return 0;
}
static inline void SetPhase(struct SceneActorRegistryResetView *a, u32 phase)
{
    u8 *state = &a->base.state2A8;
    u32 zero = 0;
    *state = zero;
    a->base.phase2A9 = zero;
    a->base.behavior2AA = phase;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
}
void Skeleton_UpdateTargetTransition(struct SceneActorRegistryResetView *a)
{
    struct SkeletonTargetContext *c = a->base.context;
    if (!TakePrepared(a)) {
        u32 flag = 0x100;
        u32 *address = &c->flags;
        u32 value = *address & flag;
        u32 *flags = address;
        if (IsActive(value)) {
            if (a->base.tracking.distance > c->timeLimit) {
                SetPhase(a, 17);
                {
                    s32 angle = a->base.tracking.heading;
                    if (angle >= 0)
                        angle >>= 5;
                    else
                        angle = -(-angle >> 5);
                    ((struct RoomActor *)a)->nextDirection = (angle + 3) & 7;
                }
            } else {
                s32 mask = ~0x100;
                *flags &= mask;
                Skeleton_SelectDistanceReaction(a);
            }
        } else
            SceneActorAnimation_SelectDirectionalEntry((struct RoomActor *)a, 1);
    }
}
