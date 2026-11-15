#include "scene_panel_rig.h"
void ScenePanelRig_TickRise(struct ScenePanelRig *rig)
{
    struct SceneRigParentView *parent = (void *)rig->parent;
    u16 frame;
    rig->colliderHold = 16;
    rig->position = rig->origin;
    frame = ++rig->stateFrame;
    if (frame > 15) {
        rig->position.y += 128;
        if (parent->mode != 0 && parent->activeCount != 0)
            ScenePanelRig_SetState(rig, 2);
        else {
            ScenePanelRig_RandomizeMovement(rig);
            rig->speed = 448;
            rig->verticalStep = 96;
            ScenePanelRig_SetState(rig, 3);
        }
    } else if (frame > 7)
        rig->position.y += 256 - ((rig->stateFrame - 8) << 4);
    else
        rig->position.y += (rig->stateFrame + 1) << 5;
}
