// MATCHING_CONCESSION: register-pinning
#include "stage_icon_display.h"
void StageIconDisplay_TickReveal(struct StageIconDisplay *display)
{
    if (!display->stateFrame)
        Sound_PlaySong(706);
    if (++display->stateFrame > 60) {
        register s32 *revealed asm("r5") = &display->revealedCount;
        if (*revealed > 2) {
            u32 *flags;
            s32 i;
            u32 hidden;
            Anim_LoadKeyframe(&display->icon, &display->resource, 50, 0);
            hidden = 1;
            flags = &display->indicators[0].flags;
            i = 2;
            do {
                *flags |= hidden;
                flags = (void *)((u8 *)flags + sizeof(struct ProjectedDisplayEntry));
            } while (--i >= 0);
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
