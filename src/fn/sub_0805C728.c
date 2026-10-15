#include "scene_panel_commands.h"
#define ScenePanel_UnregisterReceiver sub_0805C728
void sub_0805C728(struct ScenePanelCommandView *panel)
{
    sub_08020D08(&panel->receiver);
}
