#include "objects/pile_driver/pile_driver_rig.h"
void PileDriverRig_RequestSubject(struct ScenePanelRig *rig, u32 index, u32 unusedState)
{
    ((struct SceneRigParentView *)rig->parent)->subjects[index]->requested = 1;
}
