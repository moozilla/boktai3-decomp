#include "scene_inventory_control.h"
s32 SceneInventory_HandleClockShortcut(struct SceneInventoryControlView *panel)
{
    if (gUnk_03005260[0] & 4) {
        panel->interactionFlags |= 2;
        if (gUnk_03005260[1] & 1) {
            Sound_PlaySong(0xDC);
            panel->clockMode++;
            if (panel->clockMode > 2)
                panel->clockMode = 0;
        }
        return 1;
    }
    panel->interactionFlags &= 0xFD;
    return 0;
}
