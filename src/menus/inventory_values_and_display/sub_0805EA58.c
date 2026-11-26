#include "engine/shared/scene_input_panel.h"
#include "engine/shared/scene_hud_map.h"
#include "gba/syscall.h"
void SceneInputPanel_InitBackground(struct SceneInputPanel *panel, s32 unused, u32 meter)
{
    s32 mapIndices[1];
    panel->backgroundResource = ScriptProc_Start(0xC091, 0xA597);
    mapIndices[0] = 0;
    Bg_SetupLayers(0, 0, panel->backgroundResource, 0, 0, 1, mapIndices);
    Bg_SetLayerMap(0, panel->backgroundResource, 12);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    panel->labelResource = (const u8 *)ScriptProc_Start(0x92B3, 0x81B) + 0x1B4;
    CpuCopy32(panel->labelResource, gBgPaletteBank13, 3 * 32);
    SceneHud_FillTile001(8, 10, 3);
    SceneHud_FillTile001(8, 11, 3);
    SceneHud_FillTile001(10, 12, 3);
    SceneHud_FillTile001(10, 13, 3);
    SceneHud_FillTile001(10, 14, 3);
    SceneHud_DrawCenteredMeter(meter);
}
