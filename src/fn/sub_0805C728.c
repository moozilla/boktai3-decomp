#include "scene_panel_commands.h"
void ScenePanel_UnregisterReceiver(struct ScenePanelCommandView *panel)
{
    SceneMessages_UnregisterReceiver(&panel->receiver);
}
