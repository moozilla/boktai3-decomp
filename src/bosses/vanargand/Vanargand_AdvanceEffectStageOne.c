#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_AdvanceEffectStageOne(struct VanargandActionSequence *sequence)
{
    if (sequence->step == 1)
        sequence->step = 2;
}
