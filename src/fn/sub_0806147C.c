// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_control.h"
#define SceneInventory_SelectActionDestination sub_0806147C
void SceneInventory_SelectActionDestination(struct SceneInventoryControlView *panel)
{
    register u32 destination asm("r2") = (u16)panel->destination;
    panel->grid.selection = destination;
    panel->grid.cursorX = (destination & 3) + 4;
    panel->grid.cursorY = panel->grid.selection >> 2;
}
