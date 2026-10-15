#include "scene_panel.h"
#define ScenePanel_BeginLongSequence sub_080582B8
void ScenePanel_BeginLongSequence(struct ScenePanel *panel)
{
    sub_08058130(panel, 2, 6);
    sub_08058150(panel, sub_080582EC);
    panel->transitionComplete = 0;
    panel->stateFrame = 0;
}
