#include "scene_inventory_actions.h"
void SceneInventory_RefreshItemValue(struct SceneInventoryActionView *panel)
{
    struct SceneInputPosition position;
    s32 kind, value, label;
    if (panel->selection <= 15) {
        kind = sub_0822CDA8(panel->selection);
        if (kind < 0)
            goto hidden;
        value = SceneItemValue_CalculateItem(panel->selection);
        label = 1;
    } else if (panel->selection <= 23) {
        s32 index = (panel->page << 3) - 16;
        index += panel->selection;
        kind = panel->catalog[index];
        if (kind < 0)
            goto hidden;
        value = SceneItemValue_GetItemBaseValue(kind);
        label = 0;
    } else {
        kind = -1;
        value = 0;
        label = 0;
    }
    if (kind >= 0) {
        SceneInput_GetGridPosition(&position, panel->selection);
        SceneHud_ShowFourDigits((void *)panel, (void *)&position, value, label);
    } else {
    hidden:
        SceneHud_HideFourDigits((void *)panel);
    }
}
