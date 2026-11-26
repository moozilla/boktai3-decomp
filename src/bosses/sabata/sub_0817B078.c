#include "bosses/sabata/sabata_attack_state.h"

void Sabata_ResetAttackSequence(struct SabataAttackActor *volume)
{
    if (volume->sequenceMode) {
        volume->sequenceTimer = 0;
        volume->sequenceMode = 0;
        volume->sequenceLength = 0x34;
        volume->sequence = volume->sequenceBuffer;
    }
}
