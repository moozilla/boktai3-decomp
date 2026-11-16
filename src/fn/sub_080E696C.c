#include "global.h"
#include "scene_actor_reset.h"
s32 Callback_InvokeActor(void *, void *);
static inline u8 TakeEntry(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
void SceneActor_UpdateAlternateEntryPhase(struct ChaseActor *actor)
{
    if (TakeEntry(&actor->base.field2B1)) {
        void *target = actor->base.onAlternateEntry;
        u32 one = 1;
        u32 zero;
        u32 *flags;
        s32 mask;
        u8 *prepared = &actor->base.field2B2;
        zero = 0;
        *prepared = one;
        actor->base.field2B0 = zero;
        actor->base.field2B5 = one;
        actor->base.target = target;
        actor->base.field2C8 = zero;
        flags = &actor->base.flags114;
        mask = ~1;
        *flags &= mask;
        actor->base.word104 = zero;
    }
    if (actor->base.field2B0) {
        u32 behavior = 15;
        u32 zero;
        u8 *state = &actor->base.state2A8;
        zero = 0;
        *state = zero;
        actor->base.phase2A9 = zero;
        actor->base.behavior2AA = behavior;
        actor->base.counter2AC = zero;
        actor->base.field2B1 = 1;
    } else {
        Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    }
}
