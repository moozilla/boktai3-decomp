// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
struct SceneActorRegistryManager *sub_08072B44(void);
s32 SceneActorRegistry_CountLiving(void)
{
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
    s32 count;
    sub_08072B44();
    /* The allocated terminal node carries a NULL actor; an absent list head
     * is outside this helper's caller contract. */
    node = gUnk_020004AC;
    actor = node->actor;
    count = 0;
    while (actor) {
        if (actor->health > 0) {
            register u32 offset asm("r5") = 0x2d0;
            register u32 *field asm("r0");
            asm("" : "+r"(offset));
            field = (void *)((u8 *)actor + offset);
            if (!(*field & 0x20000))
                count++;
        }
        node = node->next;
        actor = node->actor;
    }
    return count;
}
