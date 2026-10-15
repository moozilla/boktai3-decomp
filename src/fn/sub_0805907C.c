#include "scene_panel_schedule.h"
#define ScenePanel_RunScheduledAction sub_0805907C
void ScenePanel_RunScheduledAction(struct ScenePanel *input)
{
    struct ScenePanelScheduleView *panel = (void *)input;
    if (panel->action)
        sub_0824923C(panel, panel->action);
}
