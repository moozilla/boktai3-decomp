#include "scene_arc.h"
struct SceneArcSlot *SceneArc_FindFreeSlot(struct SceneArcManager *manager, s32 *out)
{
    struct SceneArcSlot *slot = manager->slots;
    s32 index = 0;
    u32 one = 1, mask = manager->activeMask;
    do {
        if ((one << index) & mask) {
            index++;
            slot++;
        } else {
            *out = index;
            return slot;
        }
    } while (index <= 7);
    return 0;
}
