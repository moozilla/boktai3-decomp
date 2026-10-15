#include "scene_input_panel.h"
#include "scene_hud_map.h"
void SceneInputPanel_InitBackground(struct SceneInputPanel *panel, s32 unused, u32 meter)
{
    s32 zero;
    panel->backgroundResource = ScriptProc_Start(0xC091, 0xA597);
    zero = 0;
    sub_082161B4(0, 0, panel->backgroundResource, 0, 0, 1, &zero);
    sub_082164AC(0, panel->backgroundResource, 12);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    panel->labelResource = (const u8 *)ScriptProc_Start(0x92B3, 0x81B) + 0x1B4;
    CpuSet(panel->labelResource, gUnk_03005160, 0x04000018);
    SceneHud_FillTile001(8, 10, 3);
    SceneHud_FillTile001(8, 11, 3);
    SceneHud_FillTile001(10, 12, 3);
    SceneHud_FillTile001(10, 13, 3);
    SceneHud_FillTile001(10, 14, 3);
    SceneHud_DrawCenteredMeter(meter);
}
