#include "ui/hud/stage_icon_display.h"
void StageIconDisplay_TickDelay(struct StageIconDisplay *display)
{
    if (!display->stateFrame)
        Sound_PlaySong(1260);
    if (++display->stateFrame > 89) {
        StageIconDisplay_NotifyCompletion(display);
        Task_Kill(display);
    }
}
