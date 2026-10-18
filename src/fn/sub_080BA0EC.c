#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_script.h"

void SceneActorReaction_ApplyThreshold(struct SceneActorRegistryResetView *actor, s32 value)
{
    struct SceneActorScriptPropertiesView *context = actor->context;

    if (SceneActorControl_TakeFlag(&actor->field2B2))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);

    if (value > context->parameter706)
        actor->field2B0 = 1;
}
