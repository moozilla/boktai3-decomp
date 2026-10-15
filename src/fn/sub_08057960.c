#include "scene_panel_effects.h"
#define ScenePanel_SetEffectState sub_08057960
void ScenePanel_SetEffectState(struct ScenePanel *input, u16 state)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    panel->effectState = state;
    panel->effectFrame = 0;
}
