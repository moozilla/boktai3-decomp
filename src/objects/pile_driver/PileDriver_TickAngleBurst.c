#include "objects/pile_driver/pile_driver_schedule.h"
void PileDriver_TickAngleBurst(struct PileDriver *input)
{
    struct PileDriverScheduleView *panel = (void *)input;
    if (panel->frame < panel->interval) {
        if (++panel->frame >= panel->interval) {
            u32 angle = Math_Atan2(gGameState->positionX - panel->origin.x,
                                   gGameState->positionZ - panel->origin.z) -
                        32;
            u16 *destination = &panel->angle;
            *destination = angle & 255;
            panel->step = 0;
        }
    } else {
        if (panel->step == 0)
            Sound_PlaySong(994);
        if (panel->step <= 4) {
            DirectionBurst_Emit(&panel->origin, panel->angle, 12, 100);
            panel->angle = (panel->angle + 16) & 255;
            panel->step++;
        } else
            panel->frame = 0;
    }
}
