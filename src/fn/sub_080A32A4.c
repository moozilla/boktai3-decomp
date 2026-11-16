#include "scene_actor_reaction.h"
#include "engine.h"
/* SceneActorControl_UpdateEntryWait: tentative */

static inline u8 TakeEntry(struct ChaseActor *s)
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
static inline void EntryControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    ClearFlag(&actor->base.pendingControlFlags, ~1);
    actor->base.pendingControl = 0;
}

void SceneActorControl_UpdateEntryWait(struct ChaseActor *s)
{
    if (TakeEntry(s)) {
        void *fn = s->base.onEntry;
        u32 two = 2;
        u8 *ready = &s->base.field2B2;
        u32 zero = 0;
        *ready = 1;
        s->base.stepDone = zero;
        s->base.controlKind = two;
        s->base.target = fn;
        s->base.field2C8 = zero;
        ClearFlag(&s->base.pendingControlFlags, ~1);
        s->base.pendingControl = zero;
    } else {
        u8 result = Callback_InvokeActor(s, s->base.contactQuery.opaque);
        if (result == 0) {
            if (s->base.stepDone) {
                u32 seven = 7;
                s->base.state2A8 = result;
                s->base.phase2A9 = result;
                s->base.behavior2AA = seven;
                s->base.counter2AC = result;
                s->base.field2B1 = 1;
                EntryControl(s, 1, s->base.onAlternateEntry);
            } else {
                s->base.counter2AC++;
            }
        }
    }
}
