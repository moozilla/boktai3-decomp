#include "scene_inventory_actions.h"
void SceneInventory_RefreshEquipmentValue(struct SceneInventoryActionView *panel)
{
    struct SceneInputPosition position;
    s32 kind, value, label;
    switch (panel->valueMode) {
        case 4:
        case 5:
            if (panel->selection <= 15) {
                kind = sub_0822D6E0(panel->selection);
                value = SceneItemValue_CalculateEquipment(panel->selection);
                label = 1;
                goto ready;
            } else if (panel->selection <= 23) {
                s32 index = (panel->page << 3) - 16;
                index += panel->selection;
                kind = panel->catalog[index];
                if (kind >= 0) {
                    value = sub_08227F5C(kind)->baseValue;
                    label = 0;
                    goto ready;
                }
            }
            break;
        default:
            break;
    }
    kind = 255;
    value = 0;
    label = 0;
ready:
    if (kind != 255 && value > 0) {
        SceneInput_GetGridPosition(&position, panel->selection);
        SceneHud_ShowFourDigits((void *)panel, (void *)&position, value, label);
    } else
        SceneHud_HideFourDigits((void *)panel);
}
