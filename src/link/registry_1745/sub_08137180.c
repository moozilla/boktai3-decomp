#include "engine/shared/global.h"
#include "engine/shared/scene_actor_terrain_reaction.h"
#include "engine/shared/scene_actor_control_phase.h"

// confidence=low: scene actor wait state: on entry retargets the saved target; after 60 idle frames switches to behaviour 2.
void sub_08137180(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeStateEntry(actor)) {
        void *saved = actor->base.onStanding;
        u32 zero = 0;
        actor->base.field2B2 = 1;
        actor->base.stepDone = zero;
        actor->base.controlKind = zero;
        actor->base.target = saved;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            s32 mask = ~1;
            *flags &= mask;
        }
        actor->base.pendingControl = zero;
    }
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        if ((s32)actor->base.counter2AC > 59)
            SceneActorTerrain_EnterState(actor, 0, 0, 2);
        else
            actor->base.counter2AC++;
    }
}
