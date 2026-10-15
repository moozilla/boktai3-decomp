#include "scene_panel_schedule.h"
#define ScenePanel_TickAngleBurst sub_0805911C
void ScenePanel_TickAngleBurst(struct ScenePanel *input)
{
    struct ScenePanelScheduleView *panel = (void *)input;
    if (panel->frame < panel->interval) {
        if (++panel->frame >= panel->interval) {
            u32 angle = Math_Atan2(gUnk_02000710->positionX - panel->origin.x,
                                   gUnk_02000710->positionZ - panel->origin.z) -
                        32;
            u16 *destination = &panel->angle;
            *destination = angle & 255;
            panel->step = 0;
        }
    } else {
        if (panel->step == 0)
            Sound_PlaySong(994);
        if (panel->step <= 4) {
            sub_081A376C(&panel->origin, panel->angle, 12, 100);
            panel->angle = (panel->angle + 16) & 255;
            panel->step++;
        } else
            panel->frame = 0;
    }
}
