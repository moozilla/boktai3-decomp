#include "scene_item_values.h"
#include "debug.h"
s32 SceneItemValue_CalculateEquipment(u32 slot)
{
    struct SceneValueEquipment *equipment = Inventory_GetEquipmentEntry(slot);
    s32 percent;
    s32 result;
    s32 comparison;
    if (*(volatile u8 *)&equipment->kind == 0xFF ||
        !(result = Equipment_GetDefinition(equipment->kind)->baseValue))
        return 0;
    percent = 80;
    if (!equipment->condition)
        percent = 20;
    else if (equipment->condition <= 250)
        percent = 50;
    else if (equipment->condition <= 500) {
        DEBUG_PRINT();
    } else if (equipment->condition <= 750)
        percent = 90;
    else if (equipment->condition <= 1000)
        percent = 100;
    if (equipment->modifierA || equipment->modifierB)
        percent += 30;
    switch ((s32)equipment->grade) {
        case 0:
        default:
            break;
        case 1:
            percent += 10;
            break;
        case 2:
            percent += 20;
            break;
    }
    result = Div(result * percent, 100);
    comparison = result;
    if (comparison <= 0)
        return 1;
    return result;
}
