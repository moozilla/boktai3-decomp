#include "scene_actor_registry.h"
void SceneActorRegistry_AdvanceCursor(struct SceneActorRegistryManager *manager)
{
    if (!(manager->frame & 1)) {
        struct SceneActorRegistryNode *node = manager->cursor;
        if (node && node->actor) {
            u8 *state = (u8 *)node->actor + 0xDC;
            u8 mask = 2;
            state[0xF] = mask | state[0xF];
            manager->cursor = node->next;
        } else
            manager->cursor = gSceneActorRegistryNode;
    }
}
