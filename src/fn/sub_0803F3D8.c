#include "scene_arc.h"
void SceneArc_DestroySlot(struct SceneArcManager *, struct SceneArcSlot *, s32);
void sub_0821A2AC(u32);
u32 SceneArc_DestroyManager(struct SceneArcManager *manager)
{
    struct SceneArcSlot *slot = manager->slots;
    s32 index = 0;
    do {
        SceneArc_DestroySlot(manager, slot, index);
        index++;
        slot++;
    } while (index <= 7);
    sub_0821A2AC(0xfe2c);
    return 0;
}
