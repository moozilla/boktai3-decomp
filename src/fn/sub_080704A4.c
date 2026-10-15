// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#define SceneActorRegistry_FindLiving sub_080704A4
struct SceneActorRegistryManager *sub_08072B44(void);
struct SceneActorRegistryActor *SceneActorRegistry_FindLiving(void)
{
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
    sub_08072B44();
    /* The allocated terminal node carries a NULL actor; an absent list head
     * is outside this helper's caller contract. */
    node = gUnk_020004AC;
    actor = node->actor;
    while (actor) {
        if (actor->health > 0) {
            register u32 offset asm("r4") = 0x2d0;
            register u32 *field asm("r0");
            field = (void *)((u8 *)actor + offset);
            if (!(*field & 0x20000))
                return actor;
        }
        node = node->next;
        actor = node->actor;
    }
    return NULL;
}
