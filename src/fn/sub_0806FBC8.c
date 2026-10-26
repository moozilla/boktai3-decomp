#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorRegistry_UnlinkActor(struct SceneActorRegistryActor *actor)
{
    struct SceneActorRegistryManager *manager = SceneActorRegistry_GetManager();
    if (manager) {
        struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
        if (node) {
            struct SceneActorRegistryNode *next;
            if (node->actor == actor) {
                gSceneActorRegistryNode = node->next;
                manager->cursor = node->next;
                Heap_Free(node);
                goto removed;
            }
            goto test;
        found: {
            struct SceneActorRegistryNode *after = next->next;
            Heap_Free(next);
            (*(&node))->next = after;
            manager->cursor = after;
        }
        removed:
            manager->field32--;
            goto done;
            DEBUG_NOP();
        scan:
            next = node->next;

            if (next->actor == actor) {
                goto found;
            }
            node = next;
        test:
            if (node->next) {
                goto scan;
            }

        done:;
        }
    }
}
