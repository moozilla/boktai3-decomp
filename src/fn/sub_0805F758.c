#include "scene_input_panel.h"
#define SceneInputPanel_AdjustSlidingY sub_0805F758
s32 SceneInputPanel_AdjustSlidingY(struct SceneInputWidget *widget, s32 y, s32 frame)
{
    if (frame <= 11) {
        if ((frame & 2) == 0) {
            widget->position.y = y - ((20 - frame) >> 3);
            return 0;
        }
    } else if (frame > 15)
        return 1;
    widget->position.y = y;
    return 0;
}
