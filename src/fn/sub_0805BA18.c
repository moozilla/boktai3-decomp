#include "scene_panel_interaction.h"
#define ScenePanel_WaitTetherDirectedState sub_0805BA18
void ScenePanel_WaitTetherDirectedState(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    if (gUnk_02000488->state == 13) {
        sub_080595AC(&panel->rig, 1);
        sub_08057960(input, 10);
    }
}
