#include "scene_panel_rig.h"
u32 ScenePanelRig_GetParentMode(struct ScenePanelRig *rig)
{
    return ((struct SceneRigParentView *)rig->parent)->mode;
}
