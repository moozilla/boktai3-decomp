#include "scene_item_values.h"
s32 SceneItemValue_CalculateConsumable(u32 slot)
{
    return Div(SceneItemValue_GetConsumableBaseValue(sub_0822E104(slot)) * 50, 100);
}
