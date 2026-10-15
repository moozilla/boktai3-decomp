#include "scene_panel.h"
#define ScenePanel_TickPulseTwelve sub_0805849C
void ScenePanel_TickPulseTwelve(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        sub_08058130(panel, 2, 13);
        sub_08058150(panel, sub_080584D4);
    }
}
