#include "scene_actor_registry.h"
/* SceneActorRegistry_CountLivingByCategoryAndField1E6: Counts living actors matching category and halfword 1E6 while excluding status flag 0x20000; field meaning remains tentative */
#define SceneActorRegistry_CountLivingByCategoryAndField1E6 sub_08070444
#define SceneActorRegistry_CountLivingByCategoryAndField    sub_08070444
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
/* Byte98 and halfword1E6 are caller-supplied classification filters; the
 * field1E6 interpretation remains tentative. The registry has a NULL-actor
 * terminal node and requires an allocated head. */
s32 SceneActorRegistry_CountLivingByCategoryAndField(s32 inputCategory, s32 inputField)
{
    s32 category = inputCategory;
    s32 field = inputField;
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
    s32 count;
    SceneActorRegistry_GetManager();
    node = gSceneActorRegistryNode;
    actor = node->actor;
    count = 0;
    if (actor) {
        u32 mask = 0x20000;
        do {
            if (actor->health > 0) {
                u32 offset = 0x2d0;
                u32 *flags;
                flags = (void *)((u8 *)actor + offset);
                if (!(*flags & mask) && actor->category == category) {
                    offset -= 0xea;
                    {
                        u16 *criterion = (void *)(((u8 *)actor) - (-offset));
                        if ((*criterion) == field) {
                            count++;
                        }
                    }
                }
            }
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
    return count;
}
