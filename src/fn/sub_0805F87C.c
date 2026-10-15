#include "scene_item_values.h"
#define SceneItemValue_CalculateItem sub_0805F87C
s32 SceneItemValue_CalculateItem(u32 slot)
{
    s32 state = sub_0816D820(slot);
    s32 percent = 50;
    s32 result;
    if (state != 394) {
        percent -= (state - 395) * 10;
        if (percent > 50)
            percent = 50;
        if (percent <= 19)
            percent = 20;
    }
    result = Div(sub_0805F86C(sub_0822CDA8(slot)) * percent, 100);
    if (result <= 0)
        result = 1;
    if (result > 9999)
        result = 9999;
    return result;
}
