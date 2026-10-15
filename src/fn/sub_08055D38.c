#include "stage_icon_display.h"
#define StageIconDisplay_TickDelay sub_08055D38
void StageIconDisplay_TickDelay(struct StageIconDisplay *display)
{
    u16 *frame = &display->stateFrame;
    if (!*frame)
        Sound_PlaySong(1260);
    (*frame)++;
    if (*frame > 89) {
        sub_08055BE0(display);
        Task_Kill(display);
    }
}
