// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#define SceneActorRegistry_RestoreScriptActorFieldAA sub_080707EC
struct SceneActorRegistryActor *sub_08070EBC(u32);
void SceneActorRegistry_RestoreScriptActorFieldAA(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    if (id) {
        struct SceneActorRegistryActor *actor = sub_08070EBC(id);
        if (actor && actor->field5DC) {
            s32 value = actor->fieldA8;
            actor->fieldAA = value;
            {
                register u32 mask asm("r0") = 2;
                register u32 offset asm("r1");
                register u16 *field asm("r2");
                offset = 0x2d6;
                field = (void *)((u8 *)actor + offset);
                mask |= *field;
                *field = mask;
            }
        }
    }
}
