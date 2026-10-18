#include "scene_panel_schedule.h"
void ScenePanel_TickHandleActionA(struct ScenePanel *input)
{
    struct ScenePanelScheduleView *panel = (void *)input;
    if (++panel->frame >= panel->interval) {
        SceneBurstEmitter_RequestCleanup(panel->handle);
        panel->step = 0;
        panel->frame = 0;
    }
}
