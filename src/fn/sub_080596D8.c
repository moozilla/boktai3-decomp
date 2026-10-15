#include "scene_panel_rig.h"
#define ScenePanelRig_IsSubjectState sub_080596D8
u32 ScenePanelRig_IsSubjectState(struct ScenePanelRig *rig, u32 index, u32 state)
{
    if (((struct SceneRigParentView *)rig->parent)->subjects[index]->state == state)
        return 1;
    return 0;
}
