#include "scene_panel_interaction.h"
#define ScenePanel_WaitInitialTether sub_0805B3CC
void ScenePanel_WaitInitialTether(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    if (sub_0805B2CC(input) != 0) {
        u32 zero;
        sub_0815B15C(SCENE_PANEL_PLAYER, 0, -1, 0, zero = 0);
        panel->variant = zero;
        ScenePanel_SetEffectState(input, 1);
    }
}
