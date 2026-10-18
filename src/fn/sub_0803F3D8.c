#include "scene_arc.h"
void SceneArc_DestroySlot(struct SceneArcManager *, struct SceneArcSlot *, s32);
void ResourceRegistry_UnregisterResource(u32);
u32 SceneArc_DestroyManager(struct SceneArcManager *manager)
{
    struct SceneArcSlot *slot = manager->slots;
    s32 index = 0;
    do {
        SceneArc_DestroySlot(manager, slot, index);
        index++;
        slot++;
    } while (index <= 7);
    ResourceRegistry_UnregisterResource(0xfe2c);
    return 0;
}
