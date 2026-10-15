#include "scene_item_values.h"
#define SceneItemValue_CalculateConsumable sub_0805F8E8
s32 SceneItemValue_CalculateConsumable(u32 slot)
{
    return Div(sub_0805F8D8(sub_0822E104(slot)) * 50, 100);
}
