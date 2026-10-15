#include "scene_panel.h"
#define ScenePanel_SetPresentationMode sub_080586B0
void ScenePanel_SetPresentationMode(struct ScenePanel *panel)
{
    panel->mode = 0x30;
}
