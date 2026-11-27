#include "bosses/sabata/sabata_attack_state.h"

s32 Sabata_TakeReachedTarget(struct SabataAttackActor *volume)
{
    s32 result;
    if (volume->reached != 1) {
        result = 0;
    } else {
        volume->reached = 0;
        result = 1;
    }
    return result;
}
