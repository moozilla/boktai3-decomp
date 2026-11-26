#include "menus/scene_panel_presentation/scene_panel_effects.h"
void ScenePanel_SetEffectState(struct ScenePanel *input, u16 state)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    panel->effectState = state;
    panel->effectFrame = 0;
}
