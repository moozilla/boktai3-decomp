// confidence=low: retained provisional behavior name from caller and state-transition evidence.
#include "scene_actor_chase.h"
// confidence=low: resumes group chase or the saved route after arrival.
static inline u32 Dog_SetMotionBits(struct SceneActorRegistryResetView *actor, s32 mask)
{
    struct ChaseFlags40 *flags = (void *)&actor->flags2D4;
    flags->bits |= mask;
    return Chase_MotionFlags(actor, 0x80);
}
static inline void Dog_SetMovementBits(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u32 *flags = &actor->flags2CC;
    *flags |= mask;
}
void Dog_UpdateArrivalAnimation(void);
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void Dog_ResumeRoute(struct SceneActorRegistryResetView *a)
{
    u8 *group = (u8 *)&a->group.common;
    u8 *context = a->context;
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, Dog_UpdateArrivalAnimation, 10);
    if ((u8)Callback_InvokeActor(a, a->contactQuery.opaque))
        return;
    if (sub_0812F3CC(a))
        return;
    if (a->field2B0) {
        if (!Chase_TryRoute(a, *(void **)(group + 0x1FC), &a->body.position,
                            (struct ContactPosition *)(context + 0x6D8))) {
            Callback_InvokeActor(a, *(void **)(group + 0x224));
            return;
        }
        if (Dog_SetMotionBits(a, 0x200)) {
            Chase_SetBehavior(a, 12);
            Chase_SetCtl(a, *(void **)(group + 0x48), 0x1A);
            {
                u16 *flags = (u16 *)(group + 0x40);
                u32 mask = 4;
                mask |= *flags;
                *flags = mask;
            }
        } else {
            struct ContactPosition *p = *(struct ContactPosition **)(group + 0x200);
            Chase_SnapToRoute(p, group + 0x74);
            (*(struct ContactPosition **)(group + 0x200))->pad = 0;
            Dog_SetMovementBits(a, 0x10);
            Chase_SetCtl(a, a->entryTarget228, 1);
            Chase_SetBehavior(a, 7);
            a->field2C2 = 0x20;
        }
    } else
        a->counter2AC++;
}
