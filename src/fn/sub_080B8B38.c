#include "global.h"
#include "scene_actor_registry.h"
#include "scene_actor_script.h"

#define SceneActorScript_TriggerCallOnCount sub_080B8B38

void sub_080B8B38(struct SceneActorRegistryActor *actor)
{
    s32 zero = 0;
    struct SceneActorScriptPropertiesView *context = actor->context;
    u16 *script = &context->scriptId6CC;
    u8 *count;
    u8 *limit;

    if (*script != 0)
    {
        count = &context->callCount6CF;
        *count += 1;
        limit = &context->callLimit6CE;
        if (*count >= *limit)
        {
            u8 z;
            Script_Call(*script, zero);
            z = 0;
            *script = 0;
            *limit = z;
            *count = z;
        }
    }
}
