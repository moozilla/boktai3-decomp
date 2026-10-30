#include "global.h"
#include "scene_actor_reset.h"
s32 Callback_InvokeActor(void *, void *);
void RouteActor_UpdateAnimationSequence(void);
static inline u8 TakeEntry(struct SceneActorRegistryResetView *s)
{
    if (s->field2B1) {
        s->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u8 *taken = &a->field2B2;
    u32 zero = 0;
    *taken = 1;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}
static inline void State(struct SceneActorRegistryResetView *a, u32 phase, u32 behavior)
{
    a->state2A8 = 0;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
void Crow_UpdateReturnAnimation(struct SceneActorRegistryResetView *s)
{
    if (TakeEntry(s)) {
        Control(s, RouteActor_UpdateAnimationSequence, 0x23);
    }
    Callback_InvokeActor(s, s->contactQuery.opaque);
    if (s->field2B0) {
        State(s, 2, 1);
    } else {
        s->counter2AC++;
    }
}
