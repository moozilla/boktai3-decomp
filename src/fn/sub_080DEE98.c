#include "global.h"
#include "scene_actor_behavior.h"
#include "scene_actor_registry.h"
#include "script_utility.h"
struct SceneActorScriptContext {
    u8 unknown0[0x6A0];
    s16 mode, firstParameter, secondParameter;
};
void SceneActorControl_CallContextScript(struct SceneActorBehaviorView *actor)
{
    s32 values[12];
    struct ScriptUtilityArguments arguments;
    struct SceneActorScriptContext *context;
    {
        u32 mask = 0x80000;
        u32 offset = 0x2D0;
        u32 *field = (void *)((u32)actor + offset);
        if (*field & mask)
            return;
    }
    context = ((struct SceneActorRegistryActor *)actor)->context;
    if (!actor->script1F4)
        return;
    values[0] = actor->position.x;
    values[1] = actor->position.y;
    values[2] = actor->position.z;
    values[3] = actor->parameters1FC[0];
    values[4] = actor->parameters1FC[1];
    values[5] = actor->parameters1FC[2];
    if (context->mode < 0) {
        values[6] = -1;
        values[7] = -1;
    } else {
        values[6] = context->firstParameter;
        values[7] = context->secondParameter;
    }
    values[8] = actor->category;
    values[9] = actor->paletteOffset;
    values[10] = actor->parameter2D8;
    values[11] = -1;
    arguments.count = 12;
    arguments.values = values;
    Script_Call(actor->script1F4, &arguments);
}
