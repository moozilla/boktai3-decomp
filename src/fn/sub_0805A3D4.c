#include "scene_panel_rig.h"
void ScenePanelRig_TickContactPause(struct ScenePanelRig *rig)
{
    u16 *counter = &rig->stateFrame;
    u32 frame = *counter + 1;
    *counter = frame;
    if ((u16)frame >= rig->contactEffect) {
        ScenePanelRig_RandomizeMovement(rig);
        ScenePanelRig_SetState(rig, 3);
    }
}
