#include "scene_actor_registry.h"
void SceneActorRegistry_SendToCategoryPaletteOffset(u8 category, u8 palette, void *data)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    if (node) {
        while (node->next) {
            struct SceneActorRegistryActor *actor = node->actor;
            if (actor->category == category) {
                if (actor->paletteOffset != palette) {
                    node = node->next;
                    continue;
                }
                {
                    void *callback;
                    callback = actor->messageCallback;
                    if (callback)
                        sub_08249240(actor, data, callback);
                }
            }
            node = node->next;
        }
    }
}
