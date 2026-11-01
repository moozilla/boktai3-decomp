#include "scene_actor_reset.h"

// confidence=low: starts the contact route toward the object named by field304.
void *SceneActorContacts_FindObjectById(u16);
void SceneActorMovement_ResolveRoutePosition(struct SceneActorRegistryActor *);
void sub_080A4798(struct SceneActorRegistryResetView *);

static inline void SetBehavior(struct SceneActorRegistryResetView *actor, u32 b)
{
    u32 behavior = b;
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = k;
    actor->target = cb;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void OrFlags(u16 *flags, u32 mask)
{
    u32 value = mask;
    value |= *flags;
    *flags = value;
}

s32 SceneActorPlacement_BeginContactRoute(struct SceneActorRegistryResetView *p)
{
    struct SceneActorRegistryControlGroup *group;
    s32 result;
    if ((s32)p->field304 <= 0) {
        result = 0;
    } else {
        group = &p->group.common;
        p->tracking.target = (u32)SceneActorContacts_FindObjectById(p->field304);
        SetBehavior(p, 12);
        SetControl(p, group->gate.control, 26);
        sub_080A4798(p);
        SceneActorMovement_ResolveRoutePosition((void *)p);
        OrFlags((u16 *)&group->gate.flags, 4);
        {
            u32 mask = 128;
            u16 *flags = &p->flags2D4;
            u32 value = *flags;
            mask |= value;
            *flags = mask;
        }
        group->gate.counter = group->gate.duration;
        result = 1;
    }
    return result;
}
