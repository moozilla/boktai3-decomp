#include "objects/pile_driver/pile_driver_schedule.h"
void PileDriver_RunScheduledAction(struct ScenePanel *input)
{
    struct ScenePanelScheduleView *panel = (void *)input;
    if (panel->action)
        Callback_InvokeActor(panel, panel->action);
}
