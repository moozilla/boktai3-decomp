// MATCHING_CONCESSION: register-pinning
#include "scene_input.h"
void SceneInput_GetGridPosition(struct SceneInputPosition *position, s32 index)
{
    if (index <= 15) {
        s32 column = index & 3;
        position->x = column * 24 + 112;
        {
            s32 row = index >> 2;
            position->y = row * 24 + 32;
        }
    } else if (index <= 23) {
        register s32 column asm("r1");
        index -= 16;
        column = 3;
        column &= index;
        position->x = column * 24 + 8;
        index >>= 2;
        position->y = index * 24 + 32;
    } else if (index == 24) {
        position->x = 216;
        position->y = 104;
    } else if (index == 25) {
        position->x = 216;
        position->y = 80;
    } else {
        position->x = 216;
        position->y = 32;
    }
}
