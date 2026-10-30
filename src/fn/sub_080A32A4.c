#include "scene_actor_reaction.h"
#include "engine.h"
/* SceneActorControl_UpdateEntryWait: tentative */

static inline u8 TakeEntry(struct SceneActorRegistryResetView *s)
{
    if (s->field2B1) {
        s->field2B1 = 0;
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
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    ClearFlag(&actor->flags114, ~1);
    actor->word104 = 0;
}

void SceneActorControl_UpdateEntryWait(struct SceneActorRegistryResetView *s)
{
    if (TakeEntry(s)) {
        void *fn = s->entryTarget224;
        u32 two = 2;
        u8 *ready = &s->field2B2;
        u32 zero = 0;
        *ready = 1;
        s->field2B0 = zero;
        s->field2B5 = two;
        s->target = fn;
        s->field2C8 = zero;
        ClearFlag(&s->flags114, ~1);
        s->word104 = zero;
    } else {
        u8 result = Callback_InvokeActor(s, s->contactQuery.opaque);
        if (result == 0) {
            if (s->field2B0) {
                u32 seven = 7;
                s->state2A8 = result;
                s->phase2A9 = result;
                s->behavior2AA = seven;
                s->counter2AC = result;
                s->field2B1 = 1;
                EntryControl(s, 1, s->entryTarget228);
            } else {
                s->counter2AC++;
            }
        }
    }
}
