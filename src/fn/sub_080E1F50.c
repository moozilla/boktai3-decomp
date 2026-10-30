#define SceneActorMotion_IsVariantTimerReady Dog_IsActionTimerExpired
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
struct SceneActorTargetMotionContext {
    u8 unknown0[0x678];
    u32 flags;
};
s32 Dog_IsActionTimerExpired(struct SceneActorRegistryResetView *);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);
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
    void *collider = a->group.route;
    struct ContactPosition *position = &a->body.position;
    struct ContactPosition *destination = &a->tracking.target->position;
    if (!Nav_IsPositionInMesh(collider, position, destination)) {
        u16 *flags = &a->flags2D4;
        u32 mask = ~0x200;
        *flags &= mask;
        return 0;
    } else {
        s32 result = Terrain_ApplyBlocked(collider, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&a->body.motion, 8);
            return 1;
        } else {
            u16 *flags = &a->flags2D4;
            u32 mask = ~0x200;
            *flags &= mask;
            return 0;
        }
    }
}
void SceneActorMotion_TestTargetTransition(struct SceneActorRegistryResetView *a)
{
    struct SceneActorTargetMotionContext *context = a->context;
    if (!TakePrepared(a)) {
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)a, 11);
        if ((u8)Dog_IsActionTimerExpired(a)) {
            u8 *tail = (u8 *)a + 0x3D4;
            SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)a, 0);
            if (!TestMovement(a))
                Callback_InvokeActor(a, *(void **)(tail + 0x224));
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
