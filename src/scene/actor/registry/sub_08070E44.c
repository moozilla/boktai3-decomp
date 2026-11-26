#include "scene/actor/scene_actor_registry.h"
/* SceneActorRegistry_FindAvailableCategoryActorAfterId: Finds the next actor after an ID with flag 2CC bit 0 clear and category in a fixed whitelist; zero starts at the head */
#define SceneActorRegistry_FindAvailableCategoryActorAfterId sub_08070E44
#define SceneActorRegistry_FindAvailableFilteredAfterId      sub_08070E44
/* The observed category whitelist is preserved without guessing game roles. */
struct ChaseActor *SceneActorRegistry_FindAvailableFilteredAfterId(u32 inputId)
{
    u32 id = inputId;
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct ChaseActor *actor = node->actor;
    if (actor) {
        u32 mask = 1;
        do {
            u32 *field = &actor->base.behaviorFlags;
            if (!(*field & mask) && !id) {
                u32 category = actor->base.category;
                if (category == 4 || category == 1 || category == 3 || category == 5 ||
                    category == 7 || category == 6 || category == 8 || category == 0x19 ||
                    category == 0x15)
                    return actor;
            }
            if (actor->base.body.id == id)
                id = 0;
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
    return NULL;
}
