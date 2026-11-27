#include "ui/widgets/scene_input.h"
void SceneInput_BeginChoice(struct AnimSprite *animSprite, void *resource, u8 *selection,
                            u32 initial, u8 *blink)
{
    animSprite->flags &= ~1;
    *selection = initial;
    if ((u8)initial)
        AnimSprite_SelectGlyph(animSprite, resource, 0x4B);
    else
        AnimSprite_SelectGlyph(animSprite, resource, 0x4C);
    if (blink)
        *blink = 0;
}
