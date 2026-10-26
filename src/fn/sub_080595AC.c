#include "scene_panel_rig.h"
void ScenePanelRig_SetState(struct ScenePanelRig *rig, u32 state)
{
    if (rig->state == 0) {
        rig->position = rig->origin;
        rig->position.y += 128;
    }
    if (state == 2 && rig->state != 2) {
        ScenePanel_SetPresentationMode(rig->parent);
        ((struct SceneRigParentView *)gScenePanel)->activated = 1;
    }
    rig->state = state;
    rig->stateFrame = 0;
    if (rig->state == 0)
        ScenePanelRig_HideSprites(rig);
    else
        ScenePanelRig_ShowSprites(rig);
}
