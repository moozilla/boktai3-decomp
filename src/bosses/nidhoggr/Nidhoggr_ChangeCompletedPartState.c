#include "global.h"
#include "bosses/nidhoggr/nidhoggr_animation.h"

s32 Nidhoggr_ChangeCompletedPartState(void *object, s32 index, s32 state)
{
    struct Nidhoggr *boss = object;

    if (!boss->rowDoneFlag[index])
        return 0;
    if (boss->partState[index] == state)
        return 0;
    boss->partState[index] = state;
    boss->rowInitFlag[index] = 0;
    boss->rowCounter[index] = 0;
    boss->rowDoneFlag[index] = 0;
    return 1;
}
