#include "scene_panel_commands.h"
void ScenePanel_RegisterReceiver(struct ScenePanelCommandView *panel)
{
    SceneMessages_RegisterReceiver(&panel->receiver, panel->actorId, 9);
}
