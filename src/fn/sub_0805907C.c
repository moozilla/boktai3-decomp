#include "scene_panel_schedule.h"
void ScenePanel_RunScheduledAction(struct ScenePanel *input)
{
    struct ScenePanelScheduleView *panel = (void *)input;
    if (panel->action)
        Callback_InvokeActor(panel, panel->action);
}
