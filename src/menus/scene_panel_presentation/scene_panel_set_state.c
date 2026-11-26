#include "menus/scene_panel_presentation/scene_panel.h"
void ScenePanel_SetState(struct ScenePanel *panel, void (*state)(struct ScenePanel *))
{
    panel->state = state;
    panel->stateFrame = 0;
}
