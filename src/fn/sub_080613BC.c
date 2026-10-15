#include "scene_inventory_control.h"
#define SceneInventory_UpdateModePrompt sub_080613BC
void SceneInventory_UpdateModePrompt(struct SceneInventoryControlView *panel, s32 mode)
{
    switch (mode) {
        case 1:
            SceneInputPanel_PositionModeCursor((void *)panel, panel->mode);
            sub_08061328(panel);
            break;
        case 2:
            SceneInputPanel_PositionModeCursor((void *)panel, panel->mode);
        case 0:
            SceneInput_SetGlyphPair(&panel->prompt, 0, -1);
            break;
        case 3:
            SceneInputPanel_PositionModeCursor((void *)panel, panel->mode);
            SceneInput_SetGlyphPair(&panel->prompt, -1, -1);
            break;
    }
}
