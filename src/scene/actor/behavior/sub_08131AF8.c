#include "global.h"
#include "scene/actor/scene_actor_control_phase.h"
u8 SceneActorReaction_BeginContactResponse(struct ChaseActor *);
// confidence=low: advances the phase counter after actor/contact callbacks allow it.
void SceneActorControl_UpdateContactWait(struct ChaseActor *actor)
{
    SceneActorControl_TakeFlag(&actor->base.field2B1);
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        if (!SceneActorReaction_BeginContactResponse(actor))
            actor->base.counter2AC++;
    }
}
