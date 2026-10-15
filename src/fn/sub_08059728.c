#include "scene_panel_rig.h"
#define ScenePanelRig_GetActiveCount sub_08059728
u32 ScenePanelRig_GetActiveCount(struct ScenePanelRig *rig)
{
    return ((struct SceneRigParentView *)rig->parent)->activeCount;
}
