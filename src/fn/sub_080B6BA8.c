
#ifdef NONMATCHING
// fuzzy: 89.5%
// foundry draft (worker best 89.5%); what differs is mostly register allocation and instruction order
#define SceneActorControl_SelectBehaviorByTargetAngle sub_080B6BA8
#include "global.h"
#include "scene_actor_timed_behavior.h"
s32 Math_Atan2(s32, s32);
u8 SceneActorControl_TryBeginNearbyBehavior16(struct SceneActorRegistryResetView *,
                                              struct SceneActorTimedBehaviorContext *, s32);
u8 sub_080B6618(struct SceneActorRegistryResetView *, struct SceneActorTimedBehaviorContext *, s32);
u8 sub_080B6884(struct SceneActorRegistryResetView *, struct SceneActorTimedBehaviorContext *, s32);
void sub_080B7C10(struct SceneActorRegistryResetView *);
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
void sub_080B6BA8(struct SceneActorRegistryResetView *s)
{
    struct SceneActorTimedBehaviorContext *context;
    struct SceneActorTargetDelta delta;
    s32 angle;
    if (!(s->flags2D4 & 0x100)) {
        context = s->context;
        if (s->distanceF0 < s->rangeF4) {
            struct SceneActorTargetDelta *difference = &delta;
            struct ContactPosition *from = &s->position;
            struct ContactPosition *to = &((struct SceneActorTargetPosition *)s->fieldDC)->position;
            difference->x = to->x - from->x;
            difference->y = to->y - from->y;
            difference->z = to->z - from->z;
            angle = Math_Atan2(delta.x, difference->z) - s->direction;
            if (angle > 128)
                angle -= 256;
            else if (angle < -128)
                angle += 256;
            if (angle < 0)
                angle = -angle;
            if (angle < s->fieldE9) {
                if (s->category == 5) {
                    if (SceneActorControl_TryBeginNearbyBehavior16(s, context, angle))
                        return;
                } else if (s->category == 6) {
                    if (sub_080B6618(s, context, angle))
                        return;
                } else if (s->category == 7) {
                    if (sub_080B6884(s, context, angle))
                        return;
                }
                if (IsFlag512Set(context))
                    return;
            } else {
                if (IsFlag512Set(context))
                    return;
            }
            sub_080B7C10(s);
        } else {
            if (IsFlag512Set(context))
                return;
            sub_080B7C10(s);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080B6BA8);
#endif
