#include "scene_panel.h"
void ScenePanel_CycleOverlayPalette(struct ScenePanel *panel)
{
    if (++panel->overlayFrame & 1) {
        u32 bank = (panel->overlayFrame >> 1) & 3;
        CpuSet((u8 *)panel->overlayPalette + bank * 32, (void *)0x03005120, 0x04000008);
    }
}
