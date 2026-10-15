#include "scene_panel_commands.h"
#define ScenePanel_RegisterReceiver sub_0805C73C
void sub_0805C73C(struct ScenePanelCommandView *panel)
{
    SceneMessages_RegisterReceiver(&panel->receiver, panel->actorId, 9);
}
