#include "scene_actor_registry.h"
void SceneActorRegistry_SendToCategory(u8 a, void *data)
{
    struct SceneActorRegistryNode *n = gSceneActorRegistryNode;
    if (n != 0) {
        while (n->next != 0) {
            struct ChaseActor *o = n->actor;
            void *callback;
            if (o->base.category == a) {
                if ((callback = o->base.onMessage) != 0)
                    sub_08249240(o, data, callback);
            }
            n = n->next;
        }
    }
}
