#include "objects/pile_driver/pile_driver_schedule.h"
void PileDriver_RunScheduledAction(struct PileDriver *input)
{
    struct PileDriverScheduleView *panel = (void *)input;
    if (panel->action)
        Callback_InvokeActor(panel, panel->action);
}
