// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
s32 SceneActorRegistry_CountAvailable(void)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct SceneActorRegistryActor *actor = node->actor;
    s32 count = 0;
    if (actor) {
        u32 mask40 = 0x40;
        u32 mask20000 = 0x20000;
        u32 mask3 = 3;
        do {
            u32 inactive;
            if (actor->flags2D4 & mask40 || actor->flags2D0 & mask20000 || actor->health <= 0)
                inactive = 1;
            else
                inactive = 0;
            asm("" : "+r"(inactive));
            if (inactive || actor->flags2CC & mask3) {
                node = node->next;
                actor = node->actor;
            } else {
                node = node->next;
                actor = node->actor;
                count++;
            }
        } while (actor);
    }
    return count;
}
