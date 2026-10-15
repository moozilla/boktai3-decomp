#include "scene_input.h"
#define SceneInput_SetGlyphPair sub_0805E31C
void SceneInput_SetGlyphPair(struct SceneInputPair *pair, s32 first, s32 second)
{
    if (first >= 0) {
        u32 glyph;
        pair->widgets[0].flags &= ~1;
        glyph = sub_0805E30C(first);
        Anim_LoadKeyframe(pair->widgets, pair, (u16)glyph, 1);
    } else
        pair->widgets[0].flags |= 1;
    if (second >= 0) {
        u32 glyph;
        pair->widgets[1].flags &= ~1;
        glyph = sub_0805E30C(second);
        Anim_LoadKeyframe(pair->widgets + 1, pair, (u16)glyph, 1);
    } else
        pair->widgets[1].flags |= 1;
}
