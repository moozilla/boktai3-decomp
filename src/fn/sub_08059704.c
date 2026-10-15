#include "scene_panel_rig.h"
void ScenePanelRig_SetSubjectState(struct ScenePanelRig *rig, u32 index, u32 state)
{
    ScenePanelSubject_SetState(((struct SceneRigParentView *)rig->parent)->subjects[index], state);
}
