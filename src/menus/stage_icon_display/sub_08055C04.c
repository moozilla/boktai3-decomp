#include "menus/stage_icon_display/stage_icon_display.h"
void StageIconDisplay_TickReveal(struct StageIconDisplay *display)
{
    do {
        if (!display->stateFrame)
            Sound_PlaySong(706);
    } while (0);
    if (++display->stateFrame > 60) {
        s32 *revealed = &display->revealedCount;
        if (*revealed > 2) {
            s32 i;
            Anim_LoadKeyframe(&display->icon, &display->resource, 50, 0);
            for (i = 0; i < 3; i++)
                display->indicators[i].flags |= 1;
            StageIconDisplay_SetUpdate(display, StageIconDisplay_TickShake);
            StageIconDisplay_NotifyCompletion(display);
        } else {
            Sound_PlaySong(540);
            {
                struct ProjectedDisplayEntry *indicator =
                    (void *)(*revealed * sizeof(struct ProjectedDisplayEntry) + (u32)display +
                             0x98);
                indicator->flags &= ~1;
            }
            (*revealed)++;
            display->stateFrame = 1;
        }
    }
}
