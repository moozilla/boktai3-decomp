#include "objects/pile_driver/pile_driver_schedule.h"
void PileDriver_TickHandleActionB(struct PileDriver *input)
{
    struct PileDriverScheduleView *panel = (void *)input;
    if (++panel->frame >= panel->interval) {
        SceneOrbitHazard_Activate(panel->handle);
        panel->step = 0;
        panel->frame = 0;
    }
}
