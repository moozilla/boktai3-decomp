#include "scene_actor_registry.h"
#define SceneActorRegistry_FindAvailableFilteredAfterId sub_08070E44
/* The observed category whitelist is preserved without guessing game roles. */
struct SceneActorRegistryActor *SceneActorRegistry_FindAvailableFilteredAfterId(u32 inputId)
{
    u32 id = inputId;
    struct SceneActorRegistryNode *node = gUnk_020004AC;
    struct SceneActorRegistryActor *actor = node->actor;
    if (actor) {
        u32 mask = 1;
        u32 offset;
        do {
            u32 *field;
            offset = 0x2cc;
            field = (void *)((u8 *)actor + offset);
            if (!(*field & mask) && !id) {
                u32 category = actor->category;
                if (category == 4 || category == 1 || category == 3 || category == 5 ||
                    category == 7 || category == 6 || category == 8 || category == 0x19 ||
                    category == 0x15)
                    return actor;
            }
            if (actor->id == id)
                id = 0;
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
    return NULL;
}
