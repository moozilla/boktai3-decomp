#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_script.h"

void SceneActorReaction_ApplyThreshold(struct ChaseActor *actor, s32 value)
{
    struct SceneActorScriptPropertiesView *context = actor->base.context;

    if (SceneActorControl_TakeFlag(&actor->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);

    if (value > context->parameter706)
        actor->base.field2B0 = 1;
}
