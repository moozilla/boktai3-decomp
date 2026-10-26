// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
void SceneActorRegistry_SendToCategoryPaletteOffset(u8 category, u8 palette, void *data)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    if (node) {
        while (node->next) {
            struct SceneActorRegistryActor *actor = node->actor;
            if (actor->category == category && actor->paletteOffset == palette) {
                void *callback;
                asm("" : : : "r2");
                callback = actor->messageCallback;
                if (callback)
                    sub_08249240(actor, data, callback);
            }
            node = node->next;
        }
    }
}
