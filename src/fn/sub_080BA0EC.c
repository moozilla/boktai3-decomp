#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_script.h"

#define SceneActorReaction_ApplyThreshold sub_080BA0EC

void sub_080BA0EC(struct SceneActorRegistryResetView *actor, s32 value)
{
    struct SceneActorScriptPropertiesView *context = actor->context;

    if (SceneActorControl_TakeFlag(&actor->field2B2))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);

    if (value > context->parameter706)
        actor->field2B0 = 1;
}
