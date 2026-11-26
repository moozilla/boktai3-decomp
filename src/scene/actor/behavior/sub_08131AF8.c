#include "global.h"
#include "scene/actor/scene_actor_control_phase.h"
u8 sub_0812F3CC(struct ChaseActor *);
// confidence=low: advances the phase counter after actor/contact callbacks allow it.
#define SceneActorControl_UpdateContactWait sub_08131AF8
void SceneActorControl_UpdateContactWait(struct ChaseActor *actor)
{
    SceneActorControl_TakeFlag(&actor->base.field2B1);
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        if (!sub_0812F3CC(actor))
            actor->base.counter2AC++;
    }
}
