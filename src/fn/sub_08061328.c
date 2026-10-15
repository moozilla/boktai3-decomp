#include "scene_inventory_control.h"
void SceneInventory_UpdateActionPrompt(struct SceneInventoryControlView *panel)
{
    switch (panel->mode) {
        case 4:
        case 5: {
            s32 action = SceneInventory_ClassifySelectedAction(panel);
            if (action) {
                switch (action) {
                    case 0:
                    default:
                        break;
                    case 1:
                        SceneInput_SetGlyphPair(&panel->prompt, 0, 7);
                        return;
                    case -1:
                        SceneInput_SetGlyphPair(&panel->prompt, 0, 6);
                        return;
                }
            }
            break;
        }
        case 2:
        default:
            break;
        case 1:
            SceneInput_SetGlyphPair(&panel->prompt, 0, 3);
            return;
    }
    SceneInput_SetGlyphPair(&panel->prompt, 0, -1);
}
