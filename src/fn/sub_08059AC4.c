// MATCHING_CONCESSION: register-pinning
#include "scene_panel_rig.h"
void ScenePanelRig_TickRise(struct ScenePanelRig *rig)
{
    struct SceneRigParentView *parent = (void *)rig->parent;
    u16 *counter;
    u32 frame;
    rig->colliderHold = 16;
    rig->position = rig->origin;
    counter = &rig->stateFrame;
    frame = *counter + 1;
    *counter = frame;
    asm("" : "+r"(counter), "+r"(frame) : : "memory");
    if ((u16)frame > 15) {
        rig->position.y += 128;
        if (parent->mode != 0 && parent->activeCount != 0)
            ScenePanelRig_SetState(rig, 2);
        else {
            ScenePanelRig_RandomizeMovement(rig);
            rig->speed = 448;
            rig->verticalStep = 96;
            ScenePanelRig_SetState(rig, 3);
        }
    } else {
        u32 truncated;
        asm("" : "+r"(frame));
        truncated = (u16)frame;
        if (truncated > 7)
            rig->position.y += 256 - ((*counter - 8) << 4);
        else
            rig->position.y += (*counter + 1) << 5;
    }
}
