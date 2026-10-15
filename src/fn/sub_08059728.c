#include "scene_panel_rig.h"
u32 ScenePanelRig_GetActiveCount(struct ScenePanelRig *rig)
{
    return ((struct SceneRigParentView *)rig->parent)->activeCount;
}
