#include "ui/town/shop_actions.h"
void Shop_RefreshEquipmentValue(struct ShopActionView *panel)
{
    struct SceneInputPosition position;
    s32 kind, value, label;
    switch (panel->valueMode) {
        case 4:
        case 5:
            if (panel->selection <= 15) {
                kind = Inventory_GetEquipmentId(panel->selection);
                value = SceneItemValue_CalculateEquipment(panel->selection);
                label = 1;
                break;
            } else if (panel->selection <= 23) {
                kind = panel->catalog[panel->page * 8 + (panel->selection - 16)];
                if (kind >= 0) {
                    value = Equipment_GetDefinition(kind)->baseValue;
                    label = 0;
                    break;
                }
            }
        default:
            kind = 255;
            value = 0;
            label = 0;
            break;
    }
    if (kind != 255 && value > 0) {
        SceneInput_GetGridPosition(&position, panel->selection);
        SceneHud_ShowFourDigits((void *)panel, (void *)&position, value, label);
    } else
        SceneHud_HideFourDigits((void *)panel);
}
