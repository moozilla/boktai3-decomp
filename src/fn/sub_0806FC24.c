#include "scene_actor_registry.h"
void SceneActorRegistry_SendBroadcast(u8 a, void *data)
{
    struct SceneActorRegistryNode *n = gUnk_020004AC;
    if (n != 0) {
        while (n->next != 0) {
            struct SceneActorRegistryActor *o = n->actor;
            void *callback;
            if (a == 0) {
                if ((callback = o->messageCallback) != 0)
                    sub_08249240(o, data, callback);
            }
            n = n->next;
        }
    }
}
