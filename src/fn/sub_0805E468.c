#include "scene_input.h"
#define SceneInput_DestroyMarkerPair sub_0805E468
void SceneInput_DestroyMarkerPair(struct SceneInputPair *pair)
{
    struct SceneInputWidget *widget = pair->widgets;
    s32 remaining;
    for (remaining = 1; remaining >= 0; remaining--) {
        Anim_Remove(widget);
        widget++;
    }
}
