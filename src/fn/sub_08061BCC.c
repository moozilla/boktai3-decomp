#include "scene_inventory_states.h"
#define SceneInventory_BeginThirdSelection sub_08061BCC
void SceneInventory_BeginThirdSelection(struct SceneInventoryControlView *panel)
{
    sub_080614B4(panel, 3);
    panel->interactionFlags |= 0x80;
    SceneInput_SetGridFlags(&panel->grid, 0x1E);
    sub_080613BC(panel, 1);
}
