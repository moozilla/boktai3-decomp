#include "scene_item_values.h"
#define SceneItemValue_GetItemBaseValue sub_0805F86C
extern const struct SceneValueItemDefinition gUnk_08E61280[];
u32 SceneItemValue_GetItemBaseValue(s32 index)
{
    return gUnk_08E61280[index].baseValue;
}
