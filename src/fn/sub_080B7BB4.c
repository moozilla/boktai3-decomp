#include "global.h"
#include "scene_actor_registry.h"
#include "scene_actor_script.h"

#define SceneActorScript_ReadPParameters sub_080B7BB4

static inline s32 SceneActorScript_GetValue(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}

void sub_080B7BB4(struct SceneActorRegistryActor *actor)
{
    struct SceneActorScriptPropertiesView *context = actor->context;

    if (Script_SeekToKeyword('P'))
    {
        s32 i;

        for (i = 0; i <= 3; i++)
            context->parameters6F8[i] = Script_GetPc() ? Script_GetValue() : 0;

        context->parameter706 = SceneActorScript_GetValue();
    }
}
