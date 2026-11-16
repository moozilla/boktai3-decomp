#include "scene_actor_availability.h"
#include "scene_actor_placement.h"
#include "scene_actor_init_actor.h"
void SceneActorAttachment_BeginReturnResponse(struct SceneActorRegistryActor *);
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void ClearBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}
s32 SceneActorBehavior_DispatchPendingFlags(struct SceneActorRegistryResetView *actor)
{
    u32 unavailable = SceneActor_ActionsBlocked((void *)actor);
    if (!unavailable && actor->flags2D6) {
        struct SceneActorRegistryResetView *self = actor;
        struct SceneActorChaseGroup *group = &actor->group;
        u16 *flags;
        if (SceneActor_TestPendingFlag((void *)actor, 4, &flags)) {
            ClearBits(flags, 0x987);
            return 1;
        }
        if (SceneActorPlacement_TestFlags(flags, 2)) {
            SetState(actor, 3, 0, 0);
            SetControl(actor, group->contactControl, 7);
            {
                s32 value = actor->fieldA8;
                actor->fieldAA = value;
            }
            ClearBits(flags, 0x983);
            return 1;
        }
        if (SceneActorPlacement_TestFlags(flags, 0x800)) {
            SceneActorAttachment_BeginReturnResponse((void *)self);
            ClearBits(flags, 0x981);
            return 1;
        }
        if (SceneActorPlacement_TestFlags(flags, 1)) {
            ClearBits(flags, 0x181);
            return 1;
        }
        if (SceneActorPlacement_TestFlags(flags, 0x80)) {
            void *callback = actor->field23C;
            if (callback)
                Callback_InvokeActor(actor, callback);
            ClearBits(flags, 0x180);
            return 1;
        }
        if (SceneActorPlacement_TestFlags(flags, 0x100)) {
            void *callback = actor->group.field5EC;
            if (callback)
                Callback_InvokeActor(actor, callback);
            ClearBits(flags, 0x180);
            return 1;
        }
    }
    return 0;
}
