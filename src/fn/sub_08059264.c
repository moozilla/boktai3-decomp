#include "scene_panel_schedule.h"
#define ScenePanel_TickHandleActionA sub_08059264
void ScenePanel_TickHandleActionA(struct ScenePanel *input)
{
    struct ScenePanelScheduleView *panel = (void *)input;
    if (++panel->frame >= panel->interval) {
        sub_0819AAD8(panel->handle);
        panel->step = 0;
        panel->frame = 0;
    }
}
