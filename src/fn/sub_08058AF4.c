#include "scene_panel.h"
void ScenePanel_InitOverlay(struct ScenePanel *panel)
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
    {
        u32 mask = 0x800;
        u16 *shadow = &gDispCntShadow;
        *shadow = mask | *shadow;
    }
    Bg_SetPriority(3, 1);
    panel->overlayPalette = (u8 *)ScriptProc_Start(0x92B3, 0xE2BA) + 0x114;
    {
        u32 level = 5;
        u8 *levels = panel->targetLevels;
        s32 i = 3;
        do {
            levels[4] = level;
            levels[0] = level;
            levels++;
        } while (--i >= 0);
    }
}
