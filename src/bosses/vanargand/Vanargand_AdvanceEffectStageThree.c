#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_AdvanceEffectStageThree(struct VanargandActionSequence *sequence)
{
    if (sequence->step == 3)
        sequence->step = 4;
}
