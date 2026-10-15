#include "scene_input.h"
void SceneInput_DestroyGlyphPair(struct SceneInputPair *pair)
{
    struct SceneInputWidget *widget = pair->widgets;
    s32 remaining;
    for (remaining = 1; remaining >= 0; remaining--) {
        Anim_Remove(widget);
        widget++;
    }
}
