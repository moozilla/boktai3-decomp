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
u8 sub_0812F3CC(struct ChaseActor *);
static inline void BeginControl(struct ChaseActor *actor, void *control, u32 kind, u32 zero)
{
    {
        u8 *field = &actor->base.field2B2;
        *field = 1;
    }
    actor->base.stepDone = zero;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = zero;
}
static inline void BeginControlZero(struct ChaseActor *actor, void *control, u32 kind)
{
    u32 zero;
    {
        u8 *field = &actor->base.field2B2;
        zero = 0;
        *field = 1;
    }
    actor->base.stepDone = zero;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = zero;
}
void SceneActorControl_UpdateEntryTransition(struct ChaseActor *actor)
{
    u8 result;
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        BeginControlZero(actor, actor->base.onEntry, 2);
    }
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        result = sub_0812F3CC(actor);
        if (!result) {
            if (actor->base.stepDone) {
                SceneActorAnimation_SelectDirectionalEntry(actor, 0);
                {
                    u32 behavior = 12;
                    u8 *field = &actor->base.behavior2AA;
                    *field = behavior;
                }
                actor->base.counter2AC = result;
                actor->base.field2B1 = 1;
                BeginControl(actor, group->gate.control, 26, result);
            } else {
                ++actor->base.counter2AC;
            }
        }
    }
}
