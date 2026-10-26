#include "scene_panel.h"
void ScenePanel_InitBackground(struct ScenePanel *panel)
{
    void *map = ScriptProc_Start(0xC091, gUnk_08605A88[panel->resourceIndex]);
    s32 id;
    panel->backgroundMap = map;
    id = 0;
    Bg_SetupLayers(3, 0, map, 0, 0, 1, &id);
    Bg_SetLayerMap(1, panel->backgroundMap, 0);
    {
        u16 *control = &gDispCntShadow;
        s32 mask = ~0x200;
        *control = mask & *control;
    }
    Bg_SetPriority(1, 0);
    *(vu16 *)0x04000050 = 0x3C42;
    *(vu16 *)0x04000052 = 0x810;
    *(vu16 *)0x04000054 = 0;
    panel->palette = (u8 *)ScriptProc_Start(0x92B3, 0xF308) + 0x194;
    CpuSet(panel->palette, (void *)0x03005140, 0x04000008);
}
