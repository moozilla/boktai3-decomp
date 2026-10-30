#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
// confidence=low: sets a timed route-return command and validates the current contact target.
struct GhoulSavedAction {
    u8 pad[8];
    u8 state, phase, behavior, padB;
    u32 duration;
    void *target;
};
static inline void SaveAction(struct GhoulSavedAction *input, u32 time, void *callback, u32 kind)
{
    struct GhoulSavedAction *action = input;
    register void *target asm("r2") = callback;
    register u32 behavior asm("r3") = kind;
    register u32 duration asm("r0") = time;
    asm("" : "+r"(action), "+r"(target), "+r"(behavior));
    action->duration = duration;
    action->target = target;
    action->state = 0;
    action->phase = 0;
    action->behavior = behavior;
}
static inline void SaveReturnAction(struct SceneActorRegistryResetView *actor, u32 time, u32 kind)
{
    struct GhoulSavedAction *action = (void *)((u8 *)actor + 0x460);
    void *target;
    register u32 behavior asm("r3");
    register u32 duration asm("r0");
    target = actor->entryTarget228;
    behavior = kind;
    duration = time;
    asm("" : "+r"(action), "+r"(target), "+r"(behavior));
    action->duration = duration;
    action->target = target;
    action->state = 0;
    action->phase = 0;
    action->behavior = behavior;
}
static inline void SetRouteFlag(u8 *group)
{
    u16 *flags = (void *)(group + 0x40);
    register u32 mask asm("r0") = 4;
    register u32 value asm("r1") = *flags;
    *flags = mask | value;
}
static inline u32 MotionFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2D4 & mask;
}
void Ghoul_ResumeContactRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *group;
    struct ContactPosition *destination;

    u16 blocked = MotionFlags(actor, 0x100);
    if (blocked)
        return;
    group = (u8 *)actor + 0x3D4;
    Chase_SetState(actor, 3, 1, blocked);
    Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);

    if (MotionFlags(actor, 0x80)) {
        actor->field2C2 = 32;
        SaveAction((void *)(group + 0x8C), 30, *(void **)(group + 0x48), 12);
        SetRouteFlag(group);
        if (MotionFlags(actor, 0x80)) {
            u16 *timer = (void *)(group + 0x44);
            *timer += 300;
            if (*timer > *(u16 *)(group + 0x42))
                *timer = *(u16 *)(group + 0x42);
        }
    } else {
        {
            u32 mask = 16;
            actor->flags2CC |= mask;
        }
        actor->field2C2 = 32;
        SaveReturnAction(actor, 60, 7);
    }
    destination = *(struct ContactPosition **)(group + 0x200);
    Chase_SnapToRoute(destination, group + 0x74);
    (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position,
                        &((struct ChaseTarget *)actor->tracking.target)->position)) {
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
    }
    return;
}
