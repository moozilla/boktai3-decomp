#include "scene_arc.h"
u32 GroundEffect_UpdateManager(struct SceneGroundManager *manager)
{
    struct SceneGroundEffect *effect = manager->head;
    while (effect != 0) {
        effect->update(effect);
        effect = effect->next;
    }
    return 0;
}
