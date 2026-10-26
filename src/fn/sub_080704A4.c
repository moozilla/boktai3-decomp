#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
struct SceneActorRegistryActor *SceneActorRegistry_FindLiving(u32 unusedCategory)
{
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
    SceneActorRegistry_GetManager();
    /* The allocated terminal node carries a NULL actor; an absent list head
     * is outside this helper's caller contract. */
    node = gSceneActorRegistryNode;
    actor = node->actor;
    while (actor) {
        if (actor->health > 0) {
            u32 offset = 0x2d0;
            u32 *field;
            field = (void *)(((u8 *)actor) - (-offset));
            if (!((*field) & 0x20000)) {
                return actor;
            }
        }
        node = node->next;
        actor = node->actor;
    }
    return NULL;
}
