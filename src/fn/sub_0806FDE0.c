// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#define SceneActorRegistry_AdvanceCursor sub_0806FDE0
void SceneActorRegistry_AdvanceCursor(struct SceneActorRegistryManager *manager)
{
    if (!(manager->frame & 1)) {
        struct SceneActorRegistryNode *node = manager->cursor;
        if (node && node->actor) {
            u8 *state = (u8 *)node->actor + 0xDC;
            u32 mask = 2;
            u32 flags;
            asm("" : "+r"(mask));
            flags = state[0xF];
            state[0xF] = mask | flags;
            manager->cursor = node->next;
        } else
            manager->cursor = gUnk_020004AC;
    }
}
