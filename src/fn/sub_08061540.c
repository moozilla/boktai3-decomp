#include "scene_inventory_states.h"
#define SceneInventory_BeginBrowse sub_08061540
void SceneInventory_BeginBrowse(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x40;
    if (panel->pageCount > 1)
        panel->interactionFlags = 0x41;
    panel->destination = sub_0806143C(panel);
    sub_080613BC(panel, 0);
    sub_0806130C(panel, (SceneInventoryCallback)sub_08061DA0);
}
