#include "objects/pile_driver/pile_driver_rig.h"
void PileDriverRig_SetSubjectState(struct ScenePanelRig *rig, u32 index, u32 state)
{
    PileDriverSubject_SetState(((struct SceneRigParentView *)rig->parent)->subjects[index], state);
}
