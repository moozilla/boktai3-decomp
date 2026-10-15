// MATCHING_CONCESSION: register-pinning
#include "scene_panel_rig.h"
#define ScenePanelRig_CheckContactTransition sub_0805A4FC
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
            register s32 dz asm("r0");
            dx -= rig->origin.x;
            dz = rig->position.z;
            dz -= rig->origin.z;
            dz -= 64;
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
