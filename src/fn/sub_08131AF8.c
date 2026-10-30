#include "global.h"
#include "scene_actor_control_phase.h"
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
// confidence=low: advances the phase counter after actor/contact callbacks allow it.
#define SceneActorControl_UpdateContactWait sub_08131AF8
void SceneActorControl_UpdateContactWait(struct SceneActorRegistryResetView *actor)
{
    SceneActorControl_TakeFlag(&actor->field2B1);
    if (!(u8)Callback_InvokeActor(actor, actor->contactQuery.opaque)) {
        if (!sub_0812F3CC(actor))
            actor->counter2AC++;
    }
}
