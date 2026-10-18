#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_FindById(u32);
void SceneActorRegistry_RestoreScriptActorFieldAA(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    if (id) {
        struct SceneActorRegistryActor *actor = SceneActorRegistry_FindById(id);
        if (actor && actor->field5DC) {
            s32 value = actor->fieldA8;
            actor->fieldAA = value;
            {
                u32 mask = 2;
                u32 offset;
                u16 *field;
                offset = 0x2d6;
                field = (void *)((u8 *)actor + offset);
                mask |= *field;
                *field = mask;
            }
        }
    }
}
