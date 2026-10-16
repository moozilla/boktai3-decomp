// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_FindAvailableAfterId(u32 id)
{
    struct SceneActorRegistryNode *node = gUnk_020004AC;
    struct SceneActorRegistryActor *actor = node->actor;
    while (actor) {
        register u32 offset asm("r5") = 0x2cc;
        register u32 *field asm("r0");
        asm("" : "+r"(offset));
        field = (void *)((u8 *)actor + offset);
        if (!(*field & 1) && !id)
            return actor;
        if (actor->id == id)
            id = 0;
        node = node->next;
        actor = node->actor;
    }
    return NULL;
}
