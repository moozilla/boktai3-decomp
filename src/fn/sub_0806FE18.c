#include "scene_actor_registry.h"
static inline u8 IsInactive(struct SceneActorRegistryActor *actor, u32 mask40, u32 mask20000)
{
    if (actor->flags2D4 & mask40 || actor->flags2D0 & mask20000 || actor->health <= 0)
        return 1;
    return 0;
}
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
            u32 inactive = IsInactive(actor, mask40, mask20000);
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
