#include "bosses/vanargand/vanargand_pair_attack.h"

void VanargandPair_UpdatePairConditions(void *);
void VanargandPair_ApplyPairTransition(void *);

void Vanargand_UpdateTimedMode10(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    if (Vanargand_ConsumeActionEntry(p) == 1)
        Vanargand_EnterAction(p, 10);
    if (p->finished) {
        VanargandPair_UpdatePairConditions(q);
        VanargandPair_ApplyPairTransition(q);
    }
    p->actionTimer++;
}
