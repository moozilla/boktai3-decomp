#include "objects/pile_driver/pile_driver.h"
void PileDriver_LoadModePalette(struct PileDriver *panel)
{
    u8 *palette = (u8 *)ScriptProc_Start(0x92B3, 0x313A) + 0x14;
    palette += PileDriver_GetPaletteOffset(panel) << 5;
    CpuSet(palette, (void *)0x03004FC0, 0x04000010);
}
