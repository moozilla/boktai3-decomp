#include "scene_panel.h"
#define ScenePanel_LoadModePalette sub_080585AC
void ScenePanel_LoadModePalette(struct ScenePanel *panel)
{
    u8 *palette = (u8 *)ScriptProc_Start(0x92B3, 0x313A) + 0x14;
    palette += sub_08058544(panel) << 5;
    CpuSet(palette, (void *)0x03004FC0, 0x04000010);
}
