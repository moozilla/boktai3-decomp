#include "scene_input_navigation.h"
u32 SceneInput_GetNavigationKeys(struct SceneInputGridView *grid)
{
    u32 pressed = gPadState[1] & 0xF0;
    if (pressed) {
        grid->repeat = 0;
        return gPadState[1];
    } else {
        u32 held = gPadState[0] & 0xF0;
        if (held) {
            grid->repeat++;
            if (grid->repeat > 9) {
                grid->repeat = 0;
                return gPadState[0];
            }
        } else {
            grid->repeat = 0;
        }
        return 0;
    }
}
