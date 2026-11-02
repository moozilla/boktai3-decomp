#include "scene_inventory_control.h"
s32 SceneInventory_ClassifyConsumableMove(struct SceneInventoryControlView *panel)
{
    s32 second;
    if (panel->grid.savedSelection <= 15) {
        s32 first =
            ((struct GameState *)((u8 *)gGameState + panel->grid.savedSelection * 2))->table160[0];
        second = ((struct GameState *)((u8 *)gGameState + panel->grid.selection * 2))->table160[0];
        if (panel->grid.savedSelection == panel->grid.selection)
            return 0;
        if (first < 0 && second < 0)
            return 0;
        if (panel->mode == 1)
            return 1;
    } else {
        if (panel->grid.selection == panel->grid.savedSelection) {
            SceneInventory_SelectActionDestination(panel);
            return 4;
        }
        if (panel->grid.selection <= 15) {
            second =
                ((struct GameState *)((u8 *)gGameState + panel->grid.selection * 2))->table160[0];
            if (second < 0)
                return 4;
        }
    }
    return 0;
}
