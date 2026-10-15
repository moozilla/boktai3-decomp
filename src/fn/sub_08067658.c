#include "scene_actor_control.h"
#define SceneActorControl_DecodeDirection sub_08067658
void SceneActorControl_DecodeDirection(const struct SceneActorControlInput *input, u8 *folded,
                                       u8 *flip)
{
    u32 quadrant = ((input->direction + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *flip = 1;
        *folded = 3 - quadrant;
    } else {
        *flip = 0;
        *folded = quadrant;
    }
}
