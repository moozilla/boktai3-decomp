#include "objects/pile_driver/pile_driver_rig.h"
u32 PileDriverRig_IsSubjectState(struct ScenePanelRig *rig, u32 index, u32 state)
{
    if (((struct SceneRigParentView *)rig->parent)->subjects[index]->state == state)
        return 1;
    return 0;
}
