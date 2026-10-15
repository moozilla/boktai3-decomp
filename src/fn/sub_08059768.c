#include "scene_panel_rig.h"
static inline s32 randomValue(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
}
#define ScenePanelRig_SelectActiveSubject sub_08059768
s32 ScenePanelRig_SelectActiveSubject(struct ScenePanelRig *rig)
{
    struct SceneRigParentView *parent = (void *)rig->parent;
    s32 count = sub_08059728(rig);
    s32 i, selected;
    if (count == 0)
        return -1;
    selected = Mod(randomValue() >> 4, count);
    for (i = 0; i < parent->subjectCount; i++) {
        if (parent->subjects[i]->state == 4) {
            if (selected == 0)
                return i;
            selected--;
        }
    }
    return -1;
}
