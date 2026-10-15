// MATCHING_CONCESSION: register-pinning
#include "scene_panel_rig.h"
#define ScenePanelRig_TickRise sub_08059AC4
void ScenePanelRig_TickRise(struct ScenePanelRig *rig)
{
    struct SceneRigParentView *parent = (void *)rig->parent;
    u16 *counter;
    u32 frame;
    rig->scale = 16;
    rig->position = rig->origin;
    counter = &rig->stateFrame;
    frame = *counter + 1;
    *counter = frame;
    asm("" : "+r"(counter), "+r"(frame) : : "memory");
    if ((u16)frame > 15) {
        rig->position.y += 128;
        if (parent->mode != 0 && parent->activeCount != 0)
            sub_080595AC(rig, 2);
        else {
            sub_080597E0(rig);
            rig->speed = 448;
            rig->verticalStep = 96;
            sub_080595AC(rig, 3);
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
