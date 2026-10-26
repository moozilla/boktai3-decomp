#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
s32 SceneActorRegistry_CountLiving(void)
{
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
    s32 count;
    SceneActorRegistry_GetManager();
    /* The allocated terminal node carries a NULL actor; an absent list head
     * is outside this helper's caller contract. */
    node = gSceneActorRegistryNode;
    actor = node->actor;
    count = 0;
    while (actor) {
        if (actor->health > 0) {
            u32 offset = 0x2d0;
            u32 *field;
            field = (void *)(((u8 *)actor) - (-offset));
            if (!((*field) & 0x20000)) {
                count++;
            }
        }
        node = node->next;
        actor = node->actor;
    }
    return count;
}
