#include "scene_input.h"
#define SceneInput_BeginChoice sub_0805E190
void SceneInput_BeginChoice(struct SceneInputWidget *widget, void *resource, u8 *selection,
                            u32 initial, u8 *blink)
{
    widget->flags &= ~1;
    *selection = initial;
    if ((u8)initial)
        sub_08219728(widget, resource, 0x4B);
    else
        sub_08219728(widget, resource, 0x4C);
    if (blink)
        *blink = 0;
}
