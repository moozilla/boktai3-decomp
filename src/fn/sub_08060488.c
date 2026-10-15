#include "scene_inventory_actions.h"
#define SceneInventory_RefreshConsumableValue sub_08060488
void SceneInventory_RefreshConsumableValue(struct SceneInventoryActionView *panel)
{
    struct SceneInputPosition position;
    s32 kind, value, label;
    if (panel->selection <= 15) {
        kind = sub_0822E104(panel->selection);
        if (kind < 0)
            goto hidden;
        value = sub_0805F8E8(panel->selection);
        label = 1;
    } else if (panel->selection <= 23) {
        s32 index = (panel->page << 3) - 16;
        index += panel->selection;
        kind = panel->catalog[index];
        if (kind < 0)
            goto hidden;
        value = sub_0805F8D8(kind);
        label = 0;
    } else {
        kind = -1;
        value = 0;
        label = 0;
    }
    if (kind >= 0 && value > 0) {
        SceneInput_GetGridPosition(&position, panel->selection);
        SceneHud_ShowFourDigits((void *)panel, (void *)&position, value, label);
    } else {
    hidden:
        SceneHud_HideFourDigits((void *)panel);
    }
}
