#include "global.h"
#include "scene_actor_reset.h"
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
void SceneActorControl_UpdateSavedControlPhase(struct ChaseActor *actor)
{
    if (TakeFlag(&actor->base.field2B1)) {
        void *callback = actor->base.onStanding;
        u32 zero = 0;
        actor->base.field2B2 = 1;
        actor->base.stepDone = zero;
        actor->base.controlKind = zero;
        actor->base.target = callback;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            s32 mask = -2;
            *flags &= mask;
        }
        actor->base.pendingControl = zero;
    } else {
        if (!actor->base.counter2AC)
            Trigger_ConsumePending((u8 *)actor + 0xC, 1);
        ++actor->base.counter2AC;
    }
}
