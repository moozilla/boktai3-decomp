#include "global.h"
#include "scene_actor_registry.h"
#include "scene_actor_script.h"


void SceneActorScript_ReadCallScriptConfig(struct SceneActorRegistryActor *actor)
{
    struct SceneActorScriptPropertiesView *context = actor->context;
    s32 found = Script_SeekToKeyword('x');
    s32 a;
    s32 b;

    if (found)
    {
        if (Script_GetPc())
            a = Script_GetValue();
        else
            a = 0;
        context->scriptId6CC = a;

        if (Script_GetPc())
            b = Script_GetValue();
        else
            b = 0;
        context->callLimit6CE = b;
        context->callCount6CF = 0;
    }
    else
    {
        context->scriptId6CC = found;
        context->callLimit6CE = found;
        context->callCount6CF = 0;
    }
}
