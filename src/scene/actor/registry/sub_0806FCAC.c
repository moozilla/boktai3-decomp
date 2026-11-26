#include "scene/actor/scene_actor_registry.h"
void SceneActorRegistry_SendToCategoryPaletteOffset(u8 category, u8 palette, void *data)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    if (node) {
        while (node->next) {
            struct ChaseActor *actor = node->actor;
            if (actor->base.category == category) {
                if (actor->base.paletteOffset != palette) {
                    node = node->next;
                    continue;
                }
                {
                    void *callback;
                    callback = actor->base.onMessage;
                    if (callback)
                        sub_08249240(actor, data, callback);
                }
            }
            node = node->next;
        }
    }
}
