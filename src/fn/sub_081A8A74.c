#include "vanargand_pair_attack.h"

void Vanargand_UpdateMode20(struct VanargandPairAttack *p)
{
    if (Vanargand_ConsumeActionEntry(p))
        Vanargand_EnterAction(p, 20);
    if (p->finished)
        Vanargand_StartAction(p, 0, 0, 11);
    else
        p->actionTimer++;
}
