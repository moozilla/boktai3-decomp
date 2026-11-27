#include "objects/pile_driver/pile_driver_rig.h"
void PileDriverRig_TickContactPause(struct PileDriverRig *rig)
{
    u16 *counter = &rig->stateFrame;
    u32 frame = *counter + 1;
    *counter = frame;
    if ((u16)frame >= rig->contactEffect) {
        PileDriverRig_RandomizeMovement(rig);
        PileDriverRig_SetState(rig, 3);
    }
}
