#include "objects/pile_driver/pile_driver.h"
#include "gba/io_reg.h"
void PileDriver_InitOverlay(struct PileDriver *panel)
{
    struct OverlayResource {
        u32 words[3];
        u8 *pointers[3];
    } resource, *original = ScriptProc_Start(0xC091, 0xAD57);
    panel->overlayMap = original;
    resource = *original;
    resource.pointers[0] += (u32)original;
    resource.pointers[1] += (u32)original;
    resource.pointers[2] += (u32)original;
    Bg_EnableDisplay(DISPCNT_BG3_ON);
    Bg_SetPriority(3, 1);
    panel->overlayPalette = (u8 *)ScriptProc_Start(0x92B3, 0xE2BA) + 0x114;
    {
        s32 i;
        for (i = 0; i < 4; i++) {
            panel->currentLevels[i] = 5;
            panel->targetLevels[i] = 5;
        }
    }
}
