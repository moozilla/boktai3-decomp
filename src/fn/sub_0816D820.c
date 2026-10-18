#define SceneInventory_GetItemConditionFrame sub_0816D820
#include "global.h"
#include "scene_item_values.h"
extern const struct SceneValueItemDefinition gUnk_08E61280[];
u32 Inventory_GetItemStateValue(u32);
s32 Div(s32, s32);
s32 sub_0816D820(u32 slot)
{
    s32 item = Inventory_GetItemId(slot);
    u32 condition;
    s32 frame;
    if (item < 0 || (condition = gUnk_08E61280[item].conditionKey) == 0)
        return 394;
    frame = Div(Inventory_GetItemStateValue(slot) << 2, condition << 5);
    if (frame > 3)
        frame = 3;
    return frame + 395;
}
