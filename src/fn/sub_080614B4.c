#include "scene_inventory_control.h"
#define SceneInventory_BeginActionSelection sub_080614B4
void SceneInventory_BeginActionSelection(struct SceneInventoryControlView *panel, s32 mode)
{
    panel->promptDelay = 100;
    switch (mode) {
        case 0:
            SceneInput_InitGrid(&panel->grid, 1, 0, 0, 28);
            break;
        case 1:
            SceneInput_InitGrid(&panel->grid, 1, 4, 0, 28);
            break;
        case 2:
            SceneInput_RestoreSelection(&panel->grid);
            break;
    }
    panel->destination = sub_0806143C(panel);
    sub_080613BC(panel, 0);
    sub_0806130C(panel, sub_080620D4);
}
