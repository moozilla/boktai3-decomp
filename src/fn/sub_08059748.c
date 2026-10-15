#include "scene_panel_rig.h"
#define ScenePanelRig_RequestSubject sub_08059748
void ScenePanelRig_RequestSubject(struct ScenePanelRig *rig, u32 index)
{
    ((struct SceneRigParentView *)rig->parent)->subjects[index]->requested = 1;
}
