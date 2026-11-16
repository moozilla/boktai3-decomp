#include "scene_actor_reaction.h"
#include "engine.h"
/* SceneActorControl_UpdateEntryWait: tentative */

static inline u8 TakeEntry(struct SceneActorRegistryResetView *s)
{
    if (s->base.field2B1) {
        s->base.field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline void ClearFlag(u32 *flags, u32 mask)
{
    *flags &= mask;
}
static inline void EntryControl(struct SceneActorRegistryResetView *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    ClearFlag(&actor->base.flags114, ~1);
    actor->base.word104 = 0;
}

void SceneActorControl_UpdateEntryWait(struct SceneActorRegistryResetView *s)
{
    if (TakeEntry(s)) {
        void *fn = s->base.entryTarget224;
        u32 two = 2;
        u8 *ready = &s->base.field2B2;
        u32 zero = 0;
        *ready = 1;
        s->base.field2B0 = zero;
        s->base.field2B5 = two;
        s->base.target = fn;
        s->base.field2C8 = zero;
        ClearFlag(&s->base.flags114, ~1);
        s->base.word104 = zero;
    } else {
        u8 result = Callback_InvokeActor(s, s->base.contactQuery.opaque);
        if (result == 0) {
            if (s->base.field2B0) {
                u32 seven = 7;
                s->base.state2A8 = result;
                s->base.phase2A9 = result;
                s->base.behavior2AA = seven;
                s->base.counter2AC = result;
                s->base.field2B1 = 1;
                EntryControl(s, 1, s->base.entryTarget228);
            } else {
                s->base.counter2AC++;
            }
        }
    }
}
