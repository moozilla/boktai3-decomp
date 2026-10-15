#include "scene_panel_rig.h"
u32 ScenePanelRig_IsSubjectState(struct ScenePanelRig *rig, u32 index, u32 state)
{
    if (((struct SceneRigParentView *)rig->parent)->subjects[index]->state == state)
        return 1;
    return 0;
}
