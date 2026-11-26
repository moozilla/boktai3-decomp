#include "ui/widgets/scene_input.h"
void SceneInput_BeginChoice(struct AnimSprite *widget, void *resource, u8 *selection,
                            u32 initial, u8 *blink)
{
    widget->flags &= ~1;
    *selection = initial;
    if ((u8)initial)
        AnimSprite_SelectGlyph(widget, resource, 0x4B);
    else
        AnimSprite_SelectGlyph(widget, resource, 0x4C);
    if (blink)
        *blink = 0;
}
