// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#define SceneActorRegistry_CountLivingByCategoryAndField sub_08070444
struct SceneActorRegistryManager *sub_08072B44(void);
/* Byte98 and halfword1E6 are caller-supplied classification filters; the
 * field1E6 interpretation remains tentative. The registry has a NULL-actor
 * terminal node and requires an allocated head. */
s32 SceneActorRegistry_CountLivingByCategoryAndField(s32 inputCategory, s32 inputField)
{
    register s32 category asm("r5") = inputCategory;
    register s32 field asm("r4") = inputField;
    register struct SceneActorRegistryNode *node asm("r2");
    register struct SceneActorRegistryActor *actor asm("r1");
    register s32 count asm("r3");
    sub_08072B44();
    node = gUnk_020004AC;
    actor = node->actor;
    count = 0;
    if (actor) {
        register u32 mask asm("r6") = 0x20000;
        do {
            if (actor->health > 0) {
                u32 offset = 0x2d0;
                u32 *flags;
                flags = (void *)((u8 *)actor + offset);
                if (!(*flags & mask) && actor->state == category) {
                    offset -= 0xea;
                    {
                        register u16 *criterion asm("r0") = (void *)((u8 *)actor + offset);
                        if (*criterion == field)
                            count++;
                    }
                }
            }
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
    return count;
}
