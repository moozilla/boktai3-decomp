#include "scene_panel_rig.h"
#define ScenePanelRig_SetSubjectState sub_08059704
void ScenePanelRig_SetSubjectState(struct ScenePanelRig *rig, u32 index, u32 state)
{
    sub_0805CB88(((struct SceneRigParentView *)rig->parent)->subjects[index], state);
}
