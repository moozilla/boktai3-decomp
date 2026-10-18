#include "scene_item_values.h"
s32 SceneItemValue_GetConsumableBaseValue(u32 index)
{
    return GameState_GetConsumableDescriptor(index)->baseValue;
}
