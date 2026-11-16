#include "global.h"
#include "large_scene_animation_controller.h"

s32 Nidhoggr_ChangeCompletedPartState(void *object, s32 index, s32 state)
{
    struct Nidhoggr *boss = object;
    u8 *base = (u8 *)boss;
    u8 *ready;
    u8 *stateBase;
    u8 *current;
    u32 zero;
    base += 0x7210;
    ready = base + index;
    if (*ready == 0)
        return 0;
    stateBase = (u8 *)boss;
    stateBase += 0x71C2;
    current = stateBase + index;
    if (*current == state)
        return 0;
    zero = 0;
    *current = state;
    boss->rowInitFlag[index] = zero;
    boss->rowCounter[index] = zero;
    *ready = zero;
    return 1;
}
