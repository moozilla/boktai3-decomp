#include "scene_inventory_control.h"
void SceneInventory_SelectActionDestination(struct SceneInventoryControlView *panel)
{
    short destination = (u16)panel->destination;
    s8 four;
    panel->grid.selection = destination;
    panel->grid.cursorX = (destination & 3) + (four = 4);
    panel->grid.cursorY = panel->grid.selection >> 2;
}
