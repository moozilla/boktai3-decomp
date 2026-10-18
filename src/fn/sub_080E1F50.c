#define SceneActorMotion_TestTargetTransition sub_080E1F50
#define SceneActorMotion_IsVariantTimerReady Dog_IsActionTimerExpired
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
struct SceneActorTargetMotionContext {
    u8 unknown0[0x678];
    u32 flags;
};
s32 Dog_IsActionTimerExpired(struct SceneActorRegistryResetView *);
s32 sub_0821F204(void *, struct ContactPosition *, struct ContactPosition *);
s32 sub_0821F7A8(void *, struct ContactPosition *, struct ContactPosition *);
static inline u8 TakePrepared(struct SceneActorRegistryResetView *a)
{
    if (a->field2B2) {
        a->field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline u8 TestMovement(struct SceneActorRegistryResetView *a)
{
    void *collider = a->routeObject;
    struct ContactPosition *position = &a->position;
    struct ContactPosition *destination = (void *)(a->fieldDC + 12);
    if (!sub_0821F204(collider, position, destination)) {
        u16 *flags = &a->flags2D4;
        u32 mask = ~0x200;
        *flags &= mask;
        return 0;
    } else {
        s32 result = sub_0821F7A8(collider, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&a->motion, 8);
            return 1;
        } else {
            u16 *flags = &a->flags2D4;
            u32 mask = ~0x200;
            *flags &= mask;
            return 0;
        }
    }
}
void sub_080E1F50(struct SceneActorRegistryResetView *a)
{
    struct SceneActorTargetMotionContext *context = a->context;
    if (!TakePrepared(a)) {
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)a, 11);
        if ((u8)Dog_IsActionTimerExpired(a)) {
            u8 *tail = (u8 *)a + 0x3D4;
            SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)a, 0);
            if (!TestMovement(a))
                sub_0824923C(a, *(void **)(tail + 0x224));
            else {
                u32 *flags = &context->flags;
                s32 mask = -5;
                *flags &= mask;
                {
                    u32 next = 17;
                    u8 *state = &a->state2A8;
                    u32 zero = 0;
                    *state = zero;
                    *((u8 *)a + 0x2A9) = zero;
                    a->behavior2AA = next;
                    a->counter2AC = zero;
                    a->field2B1 = 1;
                }
            }
        }
    }
}
