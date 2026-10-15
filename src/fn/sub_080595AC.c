#include "scene_panel_rig.h"
#define ScenePanelRig_SetState sub_080595AC
void ScenePanelRig_SetState(struct ScenePanelRig *rig, u32 state)
{
    if (rig->state == 0) {
        rig->position = rig->origin;
        rig->position.y += 128;
    }
    if (state == 2 && rig->state != 2) {
        ScenePanel_SetPresentationMode(rig->parent);
        ((struct SceneRigParentView *)gUnk_020004A4)->activated = 1;
    }
    rig->state = state;
    rig->stateFrame = 0;
    if (rig->state == 0)
        sub_0805952C(rig);
    else
        sub_0805956C(rig);
}
