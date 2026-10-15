#include "scene_panel_schedule.h"
void ScenePanel_TickHandleActionB(struct ScenePanel *input)
{
    struct ScenePanelScheduleView *panel = (void *)input;
    if (++panel->frame >= panel->interval) {
        sub_0819ADF4(panel->handle);
        panel->step = 0;
        panel->frame = 0;
    }
}
