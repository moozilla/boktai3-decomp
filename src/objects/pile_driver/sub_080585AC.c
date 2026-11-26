#include "objects/pile_driver/scene_panel.h"
void PileDriver_LoadModePalette(struct ScenePanel *panel)
{
    u8 *palette = (u8 *)ScriptProc_Start(0x92B3, 0x313A) + 0x14;
    palette += PileDriver_GetPaletteOffset(panel) << 5;
    CpuSet(palette, (void *)0x03004FC0, 0x04000010);
}
