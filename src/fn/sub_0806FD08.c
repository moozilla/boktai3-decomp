#include "scene_actor_registry.h"
void SceneActorRegistry_SendToActorId(u8 a, void *data, u16 c)
{
    struct SceneActorRegistryNode *n = gSceneActorRegistryNode;
    if (n != 0) {
        while (n->next != 0) {
            struct SceneActorRegistryActor *o = n->actor;
            void *callback;
            if (o->category == a && o->id == c) {
                if ((callback = o->messageCallback) != 0) {
                    sub_08249240(o, data, callback);
                    return;
                }
            }
            n = n->next;
        }
    }
}
