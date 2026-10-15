// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#define SceneActorRegistry_SetScriptActorMovementMode sub_08070A40
struct SceneActorRegistryActor *sub_08070EBC(u32);
void sub_08070070(struct SceneActorRegistryActor *, s32);
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
        struct SceneActorRegistryActor *actor = sub_08070EBC(id);
        if (actor) {
            u32 enabled = HasMovementFlag(actor);
            if (enabled)
                sub_08070070(actor, mode);
        }
    }
}
