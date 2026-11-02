// MATCHING_CONCESSION: register-pinning
#include "stage_icon_display.h"
#include "ram.h"
extern const u16 gUnk_086141BC[];
void StageIconDisplay_TickShake(struct StageIconDisplay *display)
{
    if (!display->stateFrame)
        Sound_PlaySong(707);
    if (display->stateFrame < 8) {
        register u32 *rng asm("r3") = &gEffectRandomTableIndex;
        register u32 index asm("r1") = *rng + 1;
        u32 mask = 1023;
        const u16 *random;
        u32 sample;
        u32 mask3;
        index &= mask;
        *rng = index;
        random = gUnk_086141BC;
        sample = random[index];
        mask3 = 3;
        sample &= mask3;
        display->icon.position.x = sample - 2;
        index = (index + 1) & mask;
        *rng = index;
        index = index * 2 + (u32)random;
        {
            u32 verticalSample = *(u16 *)index;
            verticalSample &= mask3;
            display->icon.position.y = verticalSample - 2;
        }

    } else if (display->stateFrame == 8) {
        display->icon.position.x = 0;
        display->icon.position.y = 0;
    }
    if (++display->stateFrame > 29)
        Task_Kill(display);
}