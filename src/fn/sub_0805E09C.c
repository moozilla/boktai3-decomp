#include "scene_hud_display.h"
void SceneHud_ShowFourDigits(struct SceneHudDigitView *parent,
                             const struct ContactPosition *position, s32 value, u32 label)
{
    void *resource = parent->resource;
    struct SceneInputWidget *widget = &parent->label;
    s32 digits[4];
    s32 offset, remaining;
    s32 *digit;
    widget->flags &= ~1;
    widget->position = *position;
    Anim_LoadKeyframe(widget, resource, (u16)(label + 0xA8), 1);
    widget = parent->digits;
    digits[0] = Div(value, 1000);
    value -= digits[0] * 1000;
    digits[1] = Div(value, 100);
    value -= digits[1] * 100;
    digits[2] = Div(value, 10);
    value -= digits[2] * 10;
    digits[3] = value;
    offset = 0;
    digit = digits;
    remaining = 3;
    do {
        widget->flags &= ~1;
        Anim_LoadKeyframe(widget, resource, (u16)(*digit++ + 0xAA), 1);
        widget->position = *position;
        widget->position.x += offset;
        widget++;
        offset += 6;
        remaining--;
    } while (remaining >= 0);
}
