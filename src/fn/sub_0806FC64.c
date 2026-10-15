#include "scene_actor_registry.h"
void SceneActorRegistry_SendToCategory(u8 a, void *data)
{
    struct SceneActorRegistryNode *n = gUnk_020004AC;
    if (n != 0) {
        while (n->next != 0) {
            struct SceneActorRegistryActor *o = n->actor;
            void *callback;
            if (o->category == a) {
                if ((callback = o->messageCallback) != 0)
                    sub_08249240(o, data, callback);
            }
            n = n->next;
        }
    }
}
