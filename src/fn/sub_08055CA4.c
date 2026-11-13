#include "stage_icon_display.h"
#include "ram.h"
extern const u16 gUnk_086141BC[];
void StageIconDisplay_TickShake(struct StageIconDisplay *display)
{
    if (!display->stateFrame)
        Sound_PlaySong(707);
    if (display->stateFrame < 8) {
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        {
            u32 sample = gUnk_086141BC[gEffectRandomTableIndex];
            display->icon.position.x = (sample & 3) - 2;
        }
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        {
            u32 sample = gUnk_086141BC[gEffectRandomTableIndex];
            display->icon.position.y = (sample & 3) - 2;
        }

    } else if (display->stateFrame == 8) {
        display->icon.position.x = 0;
        display->icon.position.y = 0;
    }
    if (++display->stateFrame > 29)
        Task_Kill(display);
}