#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_UpdateMode23Transition(struct VanargandPairAttack *p)
{
    if (Vanargand_ConsumeActionEntry(p))
        Vanargand_EnterAction(p, 23);
    if (p->finished)
        Vanargand_StartAction(p, 0, 0, 1);
    p->actionTimer++;
}
