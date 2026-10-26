#include "scene_panel_schedule.h"
void ScenePanel_TickHandlePosition(struct ScenePanel *input)
{
    struct ScenePanelScheduleView *panel = (void *)input;
    if (++panel->frame >= panel->interval) {
        Math_Atan2(gGameState->positionX - panel->origin.x,
                   gGameState->positionZ - panel->origin.z);
        sub_08190B10(panel->handle, &panel->origin, gUnk_0824DEA4);
        panel->step = 0;
        panel->frame = 0;
    }
}
