#include "scene_inventory_states.h"
#define SceneInventory_BeginAlternateConsumableSelection sub_08061C04
void SceneInventory_BeginAlternateConsumableSelection(struct SceneInventoryControlView *panel)
{
    sub_080614B4(panel, 2);
    panel->interactionFlags |= 0x80;
    SceneInput_SetGridFlags(&panel->grid, 0x1E);
    sub_080613BC(panel, 1);
}
