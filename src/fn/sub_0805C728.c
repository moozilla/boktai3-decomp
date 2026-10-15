#include "scene_panel_commands.h"
void ScenePanel_UnregisterReceiver(struct ScenePanelCommandView *panel)
{
    sub_08020D08(&panel->receiver);
}
