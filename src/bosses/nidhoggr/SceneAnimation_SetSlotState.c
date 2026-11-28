#include "bosses/nidhoggr/nidhoggr_animation.h"

void SceneAnimation_SetSlotState(struct Nidhoggr *boss, s32 i, s32 state)
{
    boss->partState[i] = state;
    boss->rowInitFlag[i] = 0;
    boss->rowCounter[i] = 0;
    boss->rowDoneFlag[i] = 0;
}
