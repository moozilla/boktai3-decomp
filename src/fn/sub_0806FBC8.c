#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorRegistry_UnlinkActor(struct SceneActorRegistryActor *actor)
{
    struct SceneActorRegistryManager *manager = SceneActorRegistry_GetManager();
    if (manager) {
        struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
        if (node) {
            if (node->actor == actor) {
                gSceneActorRegistryNode = node->next;
                manager->cursor = node->next;
                Heap_Free(node);
                manager->field32--;
                return;
            }
            while (node->next) {
                struct SceneActorRegistryNode *next = node->next;
                if (next->actor != actor) {
                    node = next;
                    continue;
                }
                {
                    struct SceneActorRegistryNode *after = next->next;
                    Heap_Free(next);
                    node->next = after;
                    manager->cursor = after;
                }
                manager->field32--;
                return;
            }
        }
    }
}
