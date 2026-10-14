#include "scene_arc.h"
void sub_0821A284(u32, void *, u32);
void SceneArc_InitializeSlot(struct SceneArcManager *, struct SceneArcSlot *, s32);
u32 SceneArc_InitializeManager(struct SceneArcManager *manager)
{
    struct SceneArcSlot *slot;
    s32 index;
    sub_0821A284(0xfe2c, manager, 0);
    manager->activeMask = 0;
    manager->animationResource = ScriptProc_Start(0x922e, 0x871c);
    slot = manager->slots;
    index = 0;
    do {
        SceneArc_InitializeSlot(manager, slot, index);
        index++;
        slot++;
    } while (index <= 7);
    return 0;
}
