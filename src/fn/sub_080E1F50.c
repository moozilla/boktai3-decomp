#define SceneActorMotion_IsVariantTimerReady Dog_IsActionTimerExpired
#include "global.h"
#include "dog.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
s32 Dog_IsActionTimerExpired(struct ChaseActor *);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);
static inline u8 TakePrepared(struct ChaseActor *a)
{
    if (a->base.field2B2) {
        a->base.field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline u8 TestMovement(struct ChaseActor *a)
{
    void *collider = a->group.route;
    struct ContactPosition *position = &a->base.body.position;
    struct ContactPosition *destination = &a->base.tracking.target->position;
    if (!Nav_IsPositionInMesh(collider, position, destination)) {
        u16 *flags = &a->base.motionFlags;
        u32 mask = ~0x200;
        *flags &= mask;
        return 0;
    } else {
        s32 result = Terrain_ApplyBlocked(collider, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&a->base.body.motion, 8);
            return 1;
        } else {
            u16 *flags = &a->base.motionFlags;
            u32 mask = ~0x200;
            *flags &= mask;
            return 0;
        }
    }
}
void SceneActorMotion_TestTargetTransition(struct ChaseActor *a)
{
    struct Dog *context = a->base.context;
    if (!TakePrepared(a)) {
        SceneActorAnimation_SelectDirectionalEntry(&a->base, 11);
        if ((u8)Dog_IsActionTimerExpired(a)) {
            u8 *tail = (u8 *)a + 0x3D4;
            SceneActorAnimation_SelectDirectionalEntry(&a->base, 0);
            if (!TestMovement(a))
                Callback_InvokeActor(a, *(void **)(tail + 0x224));
            else {
                u32 *flags = &context->flags;
                s32 mask = -5;
                *flags &= mask;
                {
                    u32 next = 17;
                    u8 *state = &a->base.state2A8;
                    u32 zero = 0;
                    *state = zero;
                    *((u8 *)a + 0x2A9) = zero;
                    a->base.behavior2AA = next;
                    a->base.counter2AC = zero;
                    a->base.field2B1 = 1;
                }
            }
        }
    }
}
