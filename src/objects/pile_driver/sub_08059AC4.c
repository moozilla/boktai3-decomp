#include "objects/pile_driver/scene_panel_rig.h"
void ScenePanelRig_TickRise(struct ScenePanelRig *rig)
{
    struct ScenePanel *parent = rig->parent;
    u16 *counter;
    u32 frame;
    rig->colliderHold = 16;
    rig->position = rig->origin;
    counter = &rig->stateFrame;
    frame = *counter + 1;
    *counter = frame;
    if ((u16)frame > 15) {
        rig->position.y += 128;
        if (parent->paletteMode != 0 && parent->activeCount != 0) {
            ScenePanelRig_SetState(rig, 2);
            return;
        }
        // The loop's end note sits before the frame <= 15 label, so cse starts that
        // arm afresh: it compares the new count again and reloads stateFrame (fact 193).
        do {
            ScenePanelRig_RandomizeMovement(rig);
            rig->speed = 448;
            rig->verticalStep = 96;
            ScenePanelRig_SetState(rig, 3);
            return;
        } while (0);
    }
    if ((u16)frame > 7)
        rig->position.y += 256 - ((*counter - 8) << 4);
    else
        rig->position.y += (*counter + 1) << 5;
}
