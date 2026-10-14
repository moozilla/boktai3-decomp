#include "scene_arc.h"
void SceneArc_UpdateSlot(struct SceneArcManager *, struct SceneArcSlot *, u32);
u32 SceneArc_UpdateManager(struct SceneArcManager *manager)
{
    struct SceneArcSlot *slot = manager->slots;
    s32 index = 0;
    do {
        if (manager->activeMask & (1 << index))
            SceneArc_UpdateSlot(manager, slot, index);
        index++;
        slot++;
    } while (index <= 7);
    return 0;
}
