// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_FindById(u32);
void SceneActorRegistry_SetMovementMode(struct SceneActorRegistryActor *, s32);
static inline u8 HasMovementFlag(struct SceneActorRegistryActor *actor)
{
    register u32 mask asm("r1") = 0x100;
    register u32 offset asm("r3") = 0x2c0;
    register u16 *field asm("r0");
    field = (void *)((u8 *)actor + offset);
    if (*field & mask)
        return 1;
    return 0;
}
void SceneActorRegistry_SetScriptActorMovementMode(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    s32 mode = Script_GetKeywordValue('d', 0);
    if (id) {
        struct SceneActorRegistryActor *actor = SceneActorRegistry_FindById(id);
        if (actor) {
            u32 enabled = HasMovementFlag(actor);
            if (enabled)
                SceneActorRegistry_SetMovementMode(actor, mode);
        }
    }
}
