#include "scene_panel_interaction.h"
void ScenePanel_WaitTetherDirectedState(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    if (gUnk_02000488->state == 13) {
        ScenePanelRig_SetState(&panel->rig, 1);
        ScenePanel_SetEffectState(input, 10);
    }
}
