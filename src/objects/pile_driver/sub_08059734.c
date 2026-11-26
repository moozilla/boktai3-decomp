#include "objects/pile_driver/pile_driver_rig.h"
u32 PileDriverRig_GetParentMode(struct ScenePanelRig *rig)
{
    return ((struct SceneRigParentView *)rig->parent)->mode;
}
