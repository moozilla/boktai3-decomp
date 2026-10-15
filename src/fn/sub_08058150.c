#include "scene_panel.h"
#define ScenePanel_SetState sub_08058150
void ScenePanel_SetState(struct ScenePanel *panel, void (*state)(struct ScenePanel *))
{
    panel->state = state;
    panel->stateFrame = 0;
}
