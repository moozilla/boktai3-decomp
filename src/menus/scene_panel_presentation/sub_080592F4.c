#include "engine/shared/scene_panel_schedule.h"
void ScenePanel_InitScheduledAction(struct ScenePanel *input)
{
    struct ScenePanelScheduleView *panel = (void *)input;
    switch (panel->resourceIndex) {
        case 0:
            panel->handle = 0;
            panel->action = ScenePanel_TickHazardSpawner;
            panel->interval = 600;
            break;
        case 1:
            panel->handle = Hresvelgr_CreateShotPool(panel->resourceId, 10, 30);
            panel->action = ScenePanel_TickAngleBurst;
            panel->interval = 600;
            break;
        case 2:
            panel->handle = SceneEffect_CreateMovingTask(panel->resourceId, &panel->origin);
            panel->action = ScenePanel_TickHandleActionB;
            panel->interval = 600;
            break;
        case 3:
            panel->handle = ScenePanelEffect_Create(panel->resourceId, panel->parameter);
            panel->action = ScenePanel_TickHandlePosition;
            panel->interval = 360;
            break;
        case 4:
            panel->handle = SceneEffect_CreateEntryTask(panel->resourceId, &panel->origin);
            panel->action = ScenePanel_TickHandleActionA;
            panel->interval = 600;
            break;
        default:
            panel->action = 0;
            break;
    }
}
