#include "scene_item_values.h"
s32 SceneItemValue_CalculateConsumable(u32 slot)
{
    return Div(SceneItemValue_GetConsumableBaseValue(Inventory_GetConsumableId(slot)) * 50, 100);
}
