// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_animation.h"
#define Dog_ResumeBehaviorAfterAnimation sub_080E059C
// confidence=low: resumes chase or idle behavior after animation 11 signals completion.
void SceneActorControl_OnIdle(void);
void SceneActor_SetPositionXZ(void *, s32, s32);
static inline u8 TakePrepared(struct SceneActorRegistryResetView *a)
{
    if (a->field2B2) {
        a->field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline u16 MotionFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    u16 value = a->flags2D4;
    return value & mask;
}
static inline void OrFlags(u16 *flags, u32 mask)
{
    register u32 bits asm("r0") = mask;
    *flags = bits | *flags;
}
void Dog_ResumeBehaviorAfterAnimation(struct SceneActorRegistryResetView *a)
{
    if (TakePrepared(a)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)a, 11);
        a->field2B3 = 0;
    }
    if (a->field2B3) {
        u8 *group = (u8 *)&a->group;
        a->field2B0 = 1;
        if (MotionFlags(a, 0x80)) {
            SceneActor_SetPositionXZ((u8 *)a + 0x50,
                                     ((struct ContactCollider *)a->routeObject)->bounds.x,
                                     ((struct ContactCollider *)a->routeObject)->bounds.z);
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            Chase_SetCtl(a, *(void **)(group + 0x48), 26);
            Chase_SetBehavior(a, 12);
            OrFlags((u16 *)(group + 0x40), 4);
            a->field2C2 = 32;
        } else {
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            Chase_SetCtl(a, SceneActorControl_OnIdle, 16);
            Chase_SetBehavior(a, 7);
            a->flags2CC |= 16;
        }
    }
}
