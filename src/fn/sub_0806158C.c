#include "scene_inventory_states.h"
#define SceneInventory_BeginMoveSelection sub_0806158C
void SceneInventory_BeginMoveSelection(struct SceneInventoryControlView *panel)
{
    sub_080614B4(panel, 0);
    panel->interactionFlags |= 16;
    if (panel->pageCount > 1)
        panel->interactionFlags |= 1;
    SceneInput_SetGridFlags(&panel->grid, 0x1C);
    sub_080613BC(panel, 1);
}
