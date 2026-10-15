#include "scene_panel_schedule.h"
#define ScenePanel_TickHazardSpawner sub_0805909C
void ScenePanel_TickHazardSpawner(struct ScenePanel *input)
{
    struct ScenePanelScheduleView *panel = (void *)input;
    if (++panel->frame >= panel->interval) {
        struct ContactPackedVector position;
        position.x = gUnk_02000710->positionX;
        position.y = gUnk_02000710->positionY;
        position.z = gUnk_02000710->positionZ;
        AnimatedHazard_Create(&position, panel->resourceId, 30);
        panel->step = 0;
        panel->frame = 0;
    }
}
