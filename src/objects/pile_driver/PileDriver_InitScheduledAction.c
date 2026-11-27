#include "objects/pile_driver/pile_driver_schedule.h"
void PileDriver_InitScheduledAction(struct PileDriver *input)
{
    struct PileDriverScheduleView *panel = (void *)input;
    switch (panel->resourceIndex) {
        case 0:
            panel->handle = 0;
            panel->action = PileDriver_TickHazardSpawner;
            panel->interval = 600;
            break;
        case 1:
            panel->handle = Hresvelgr_CreateShotPool(panel->resourceId, 10, 30);
            panel->action = PileDriver_TickAngleBurst;
            panel->interval = 600;
            break;
        case 2:
            panel->handle = SceneEffect_CreateMovingTask(panel->resourceId, &panel->origin);
            panel->action = PileDriver_TickHandleActionB;
            panel->interval = 600;
            break;
        case 3:
            panel->handle = NidhoggrEffect_Create(panel->resourceId, panel->parameter);
            panel->action = PileDriver_TickHandlePosition;
            panel->interval = 360;
            break;
        case 4:
            panel->handle = SceneEffect_CreateEntryTask(panel->resourceId, &panel->origin);
            panel->action = PileDriver_TickHandleActionA;
            panel->interval = 600;
            break;
        default:
            panel->action = 0;
            break;
    }
}
