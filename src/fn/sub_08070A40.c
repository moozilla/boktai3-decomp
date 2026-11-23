#include "scene_actor_registry.h"
struct ChaseActor *SceneActorRegistry_FindById(u32);
void SceneActorRegistry_SetMovementMode(struct ChaseActor *, s32);
static inline u8 HasMovementFlag(struct ChaseActor *actor)
{
    u32 mask = 0x100;
    u16 *field;
    field = (void *)((u8 *)actor + 0x2c0);
    if (*field & mask)
        return 1;
    return 0;
}
void SceneActorRegistry_SetScriptActorMovementMode(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    s32 mode = Script_GetKeywordValue('d', 0);
    if (id) {
        struct ChaseActor *actor = SceneActorRegistry_FindById(id);
        if (actor) {
            u32 enabled = HasMovementFlag(actor);
            if (enabled)
                SceneActorRegistry_SetMovementMode(actor, mode);
        }
    }
}
