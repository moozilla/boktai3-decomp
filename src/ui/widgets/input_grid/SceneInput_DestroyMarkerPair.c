#include "ui/widgets/scene_input.h"
void SceneInput_DestroyMarkerPair(struct SceneInputPair *pair)
{
    struct AnimSprite *animSprite = pair->widgets;
    s32 remaining;
    for (remaining = 1; remaining >= 0; remaining--) {
        Anim_Remove(animSprite);
        animSprite++;
    }
}
