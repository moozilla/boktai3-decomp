#include "global.h"
#include "rotation_phase.h"
#include "scene_actor_timed_behavior.h"

s32 Math_Atan2(s32, s32);
u8 SceneActorControl_TryBeginNearbyBehavior16(struct ChaseActor *,
                                              struct SceneActorTimedBehaviorContext *, s32);
u8 Skeleton_TryBeginGuardedAttack(struct ChaseActor *, struct SceneActorTimedBehaviorContext *,
                                  s32);
u8 Skeleton_TryBeginArmedAttack(struct ChaseActor *, struct SceneActorTimedBehaviorContext *, s32);
void Skeleton_SelectDistanceReaction(struct ChaseActor *);
struct SceneActorTargetDelta {
    s16 x, y, z;
};
struct SceneActorTargetPosition {
    u8 unknown0[0xC];
    struct ContactPosition position;
};
static inline u8 IsFlag512Set(struct SceneActorTimedBehaviorContext *context)
{
    u32 mask = 0x200;
    if (context->flags6DC & mask)
        return 1;
    return 0;
}

void SceneActorControl_SelectBehaviorByTargetAngle(struct ChaseActor *s)
{
    struct SceneActorTimedBehaviorContext *context;
    struct SceneActorTargetDelta delta;
    s32 angle;
    u32 m = 0x100;

    if (!(s->base.motionFlags & m)) {
        context = s->base.context;
        if (s->base.tracking.distance < s->base.tracking.range) {
            struct SceneActorTargetDelta *difference = &delta;
            struct ContactPosition *from = &s->base.body.position;
            struct ContactPosition *to =
                &((struct SceneActorTargetPosition *)s->base.tracking.target)->position;
            difference->x = to->x - from->x;
            difference->y = to->y - from->y;
            difference->z = to->z - from->z;
            angle = RotationPhase_HeadingDistance(Math_Atan2(delta.x, difference->z),
                                                  s->base.body.direction);
            if (angle < s->base.tracking.heading) {
                if (s->base.category == 5) {
                    if (SceneActorControl_TryBeginNearbyBehavior16(s, context, angle))
                        return;
                } else if (s->base.category == 6) {
                    if (Skeleton_TryBeginGuardedAttack(s, context, angle))
                        return;
                } else if (s->base.category == 7) {
                    if (Skeleton_TryBeginArmedAttack(s, context, angle))
                        return;
                }
                if (IsFlag512Set(context))
                    return;
            } else {
                if (IsFlag512Set(context))
                    return;
            }
            Skeleton_SelectDistanceReaction(s);
        } else {
            if (IsFlag512Set(context))
                return;
            Skeleton_SelectDistanceReaction(s);
        }
    }
}
