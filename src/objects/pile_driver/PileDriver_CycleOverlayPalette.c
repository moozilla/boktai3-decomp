#include "objects/pile_driver/pile_driver.h"
void PileDriver_CycleOverlayPalette(struct PileDriver *panel)
{
    if (++panel->overlayFrame & 1) {
        u32 bank = (panel->overlayFrame >> 1) & 3;
        CpuSet((u8 *)panel->overlayPalette + bank * 32, (void *)0x03005120, 0x04000008);
    }
}
