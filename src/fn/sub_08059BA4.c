#include "scene_panel_rig.h"
void ScenePanelRig_TickReturn(struct ScenePanelRig *rig)
{
    rig->scale = 16;
    if (++rig->stateFrame > 15)
        ScenePanelRig_SetState(rig, 0);
    else {
        s32 weight = 16 - rig->stateFrame, divisor = weight + 1;
        rig->position.x = Div(rig->position.x * weight + rig->origin.x, divisor);
        rig->position.y = Div(rig->position.y * weight + rig->origin.y, divisor);
        rig->position.z = Div(rig->position.z * weight + rig->origin.z, divisor);
    }
}
