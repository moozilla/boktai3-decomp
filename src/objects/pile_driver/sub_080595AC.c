#include "objects/pile_driver/pile_driver_rig.h"
void PileDriverRig_SetState(struct PileDriverRig *rig, u32 state)
{
    if (rig->state == 0) {
        rig->position = rig->origin;
        rig->position.y += 128;
    }
    if (state == 2 && rig->state != 2) {
        PileDriver_SetPresentationMode(rig->parent);
        ((struct SceneRigParentView *)gScenePanel)->activated = 1;
    }
    rig->state = state;
    rig->stateFrame = 0;
    if (rig->state == 0)
        PileDriverRig_HideSprites(rig);
    else
        PileDriverRig_ShowSprites(rig);
}
