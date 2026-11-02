#include "scene_panel_rig.h"
static inline s32 ScenePanelRig_ContactDepth(const struct ScenePanelRig *rig)
{
    return rig->position.z - rig->origin.z - 64;
}

void ScenePanelRig_CheckContactTransition(struct ScenePanelRig *rig)
{
    u32 state = rig->state;
    if (state == 9 || state == 0 || state == 1 || state == 6)
        return;
    if (state == 2) {
        if (ScenePanelRig_GetParentMode(rig) == 0) {
            ScenePanelRig_RandomizeMovement(rig);
            ScenePanelRig_SetState(rig, 3);
            return;
        }
    } else if (state == 7) {
        if ((s32)ScenePanelRig_GetParentMode(rig) > 0) {
            s32 dx = rig->position.x;
            s32 dz;
            dx -= rig->origin.x;
            dz = ScenePanelRig_ContactDepth(rig);
            dx -= 64;
            if (dx < 0)
                dx = -dx;
            if (dx <= 127) {
                if (dz < 0)
                    dz = -dz;
                if (dz <= 127) {
                    ScenePanelRig_SetState(rig, 2);
                    rig->contactForce = 0;
                    return;
                }
            }
        }
    }
    if (rig->contactForce != 0)
        ScenePanelRig_SetState(rig, 7);
}
