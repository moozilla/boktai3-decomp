#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_UpdateEntryTransition(struct VanargandPairAttack *p)
{
    if (Vanargand_ConsumeActionEntry(p))
        Vanargand_EnterAction(p, 18);
    if (p->finished)
        Vanargand_EnterActionPhase(p, 13);
}
