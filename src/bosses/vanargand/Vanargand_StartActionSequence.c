#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_StartActionSequence(struct VanargandActionSequence *sequence)
{
    if (sequence->step == 0)
        sequence->step = 1;
}
