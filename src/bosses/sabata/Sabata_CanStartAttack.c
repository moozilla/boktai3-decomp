#include "bosses/sabata/sabata_attack_state.h"

s32 Sabata_CanStartAttack(struct SabataAttackActor *volume)
{
    if (!(volume->previousChoice == 2 || volume->previousChoice == 10))
        return 0;
    if (volume->decisionIndex != 4)
        return 0;
    if (!(volume->effectFlags & 7))
        return 0;
    if (volume->target->actionMode)
        return 0;
    if (volume->impactCount)
        return 0;
    return 1;
}
