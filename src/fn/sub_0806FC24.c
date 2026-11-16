#include "scene_actor_registry.h"
void SceneActorRegistry_SendBroadcast(u8 a, void *data)
{
    struct SceneActorRegistryNode *n = gSceneActorRegistryNode;
    if (n != 0) {
        while (n->next != 0) {
            struct ChaseActor *o = n->actor;
            void *callback;
            if (a == 0) {
                if ((callback = o->base.messageCallback) != 0)
                    sub_08249240(o, data, callback);
            }
            n = n->next;
        }
    }
}
