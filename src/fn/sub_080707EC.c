#include "scene_actor_registry.h"
struct ChaseActor *SceneActorRegistry_FindById(u32);
void SceneActorRegistry_RestoreScriptActorFieldAA(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    if (id) {
        struct ChaseActor *actor = SceneActorRegistry_FindById(id);
        if (actor && actor->group.contactControl) {
            s32 value = actor->base.attachmentRestart;
            actor->base.attachmentTimer = value;
            {
                u32 mask = 2;
                u16 *field;
                field = (void *)((u8 *)actor + 0x2d6);
                mask |= *field;
                *field = mask;
            }
        }
    }
}
