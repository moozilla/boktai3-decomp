#include "ui/town/shop_actions.h"
void Shop_RefreshConsumableValue(struct ShopActionView *panel)
{
    struct SceneInputPosition position;
    s32 kind, value, label;
    if (panel->selection <= 15) {
        kind = Inventory_GetConsumableId(panel->selection);
        if (kind < 0)
            goto hidden;
        value = SceneItemValue_CalculateConsumable(panel->selection);
        label = 1;
    } else if (panel->selection <= 23) {
        kind = panel->catalog[panel->selection - 16 + panel->page * 8];
        if (kind < 0)
            goto hidden;
        value = SceneItemValue_GetConsumableBaseValue(kind);
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
