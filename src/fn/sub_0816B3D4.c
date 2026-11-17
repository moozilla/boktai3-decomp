#include "scene_input_navigation.h"

u32 SceneInput_ReadRepeatingDirections(struct SceneInputGridView *s)
{
    if (gPadState[1] & 0xF0) {
        s->repeatTimer = 0;
        return gPadState[1];
    }
    if (gPadState[0] & 0xF0) {
        if (++s->repeatTimer > 9) {
            s->repeatTimer = 0;
            return gPadState[0];
        }
    } else {
        s->repeatTimer = 0;
    }
    return 0;
}
