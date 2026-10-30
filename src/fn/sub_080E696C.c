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
void SceneActor_UpdateAlternateEntryPhase(struct SceneActorRegistryResetView *actor)
{
    if (TakeEntry(&actor->field2B1)) {
        void *target = actor->entryTarget228;
        u32 one = 1;
        u32 zero;
        u32 *flags;
        s32 mask;
        u8 *prepared = &actor->field2B2;
        zero = 0;
        *prepared = one;
        actor->field2B0 = zero;
        actor->field2B5 = one;
        actor->target = target;
        actor->field2C8 = zero;
        flags = &actor->flags114;
        mask = ~1;
        *flags &= mask;
        actor->word104 = zero;
    }
    if (actor->field2B0) {
        u32 behavior = 15;
        u32 zero;
        u8 *state = &actor->state2A8;
        zero = 0;
        *state = zero;
        actor->phase2A9 = zero;
        actor->behavior2AA = behavior;
        actor->counter2AC = zero;
        actor->field2B1 = 1;
    } else {
        Callback_InvokeActor(actor, actor->contactQuery.opaque);
    }
}
