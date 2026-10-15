#include "scene_panel.h"
void ScenePanel_LoadModePalette(struct ScenePanel *panel)
{
    u8 *palette = (u8 *)ScriptProc_Start(0x92B3, 0x313A) + 0x14;
    palette += ScenePanel_GetPaletteOffset(panel) << 5;
    CpuSet(palette, (void *)0x03004FC0, 0x04000010);
}
