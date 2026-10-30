// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"

void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
static inline u8 SceneActorControl_TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline u32 HasFinishedControl(struct SceneActorRegistryResetView *actor)
{
    register u32 offset asm("r1") = 0x2B0;
    u8 *field;
    /* Keep the completion-field address local to this check. */
    field = (u8 *)actor + offset;
    return *field;
}
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
static inline void BeginControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind,
                                u32 zero)
{
    u32 offset;
    {
        u8 *field = &actor->field2B2;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = zero;
    actor->field2B5 = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}
static inline void BeginControlZero(struct SceneActorRegistryResetView *actor, void *control,
                                    u32 kind)
{
    u32 offset;
    u32 zero;
    {
        u8 *field = &actor->field2B2;
        zero = 0;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = zero;
    actor->field2B5 = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}
void SceneActorControl_UpdateEntryTransition(struct SceneActorRegistryResetView *actor)
{
    u8 result;
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        BeginControlZero(actor, actor->entryTarget224, 2);
    }
    if (!(u8)Callback_InvokeActor(actor, actor->contactQuery.opaque)) {
        result = sub_0812F3CC(actor);
        if (!result) {
            if (HasFinishedControl(actor)) {
                SceneActorAnimation_SelectDirectionalEntry(actor, 0);
                {
                    u32 behavior = 12;
                    u8 *field = &actor->behavior2AA;
                    *field = behavior;
                }
                actor->counter2AC = result;
                actor->field2B1 = 1;
                BeginControl(actor, group->gate.control, 26, result);
            } else {
                ++actor->counter2AC;
            }
        }
    }
}
