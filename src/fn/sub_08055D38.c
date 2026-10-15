#include "stage_icon_display.h"
void StageIconDisplay_TickDelay(struct StageIconDisplay *display)
{
    u16 *frame = &display->stateFrame;
    if (!*frame)
        Sound_PlaySong(1260);
    (*frame)++;
    if (*frame > 89) {
        StageIconDisplay_NotifyCompletion(display);
        Task_Kill(display);
    }
}
