// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_timed_behavior.h"

s32 Math_Atan2(s32, s32);
u8 SceneActorControl_TryBeginNearbyBehavior16(struct SceneActorRegistryResetView *,
                                              struct SceneActorTimedBehaviorContext *, s32);
u8 Skeleton_TryBeginGuardedAttack(struct SceneActorRegistryResetView *,
                                  struct SceneActorTimedBehaviorContext *, s32);
u8 Skeleton_TryBeginArmedAttack(struct SceneActorRegistryResetView *,
                                struct SceneActorTimedBehaviorContext *, s32);
void Skeleton_SelectDistanceReaction(struct SceneActorRegistryResetView *);
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
void SceneActorControl_SelectBehaviorByTargetAngle(struct SceneActorRegistryResetView *s)
{
    struct SceneActorTimedBehaviorContext *context;
    struct SceneActorTargetDelta delta;
    s32 angle;
    u32 m = 0x100;
    asm("" : "+r"(m));
    if (!(s->flags2D4 & m)) {
        context = s->context;
        if (s->tracking.distance < s->tracking.range) {
            struct SceneActorTargetDelta *difference = &delta;
            struct ContactPosition *from = &s->body.position;
            struct ContactPosition *to = &((struct SceneActorTargetPosition *)s->tracking.target)->position;
            difference->x = to->x - from->x;
            difference->y = to->y - from->y;
            difference->z = to->z - from->z;
            angle = Math_Atan2(delta.x, difference->z) - s->body.direction;
            if (angle > 128)
                angle -= 256;
            else if (angle < -128)
                angle += 256;
            if (angle < 0)
                angle = -angle;
            if (angle < s->tracking.heading) {
                if (s->category == 5) {
                    if (SceneActorControl_TryBeginNearbyBehavior16(s, context, angle))
                        return;
                } else if (s->category == 6) {
                    if (Skeleton_TryBeginGuardedAttack(s, context, angle))
                        return;
                } else if (s->category == 7) {
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
