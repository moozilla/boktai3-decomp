#include "scene_panel_rig.h"
#define ScenePanelRig_GetParentMode sub_08059734
u32 ScenePanelRig_GetParentMode(struct ScenePanelRig *rig)
{
    return ((struct SceneRigParentView *)rig->parent)->mode;
}
