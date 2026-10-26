#include "scene_panel_schedule.h"
void ScenePanel_TickHazardSpawner(struct ScenePanel *input)
{
    struct ScenePanelScheduleView *panel = (void *)input;
    if (++panel->frame >= panel->interval) {
        struct ContactPackedVector position;
        position.x = gGameState->positionX;
        position.y = gGameState->positionY;
        position.z = gGameState->positionZ;
        AnimatedHazard_Create(&position, panel->resourceId, 30);
        panel->step = 0;
        panel->frame = 0;
    }
}
