#include "bosses/vanargand/vanargand_pair_attack.h"

void VanargandPair_UpdatePairConditions(void *);
void VanargandPair_ApplyPairTransition(void *);

void ActorSoundPhase_UpdateStop(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    if (Vanargand_ConsumeActionEntry(p))
        Vanargand_EnterAction(p, 15);
    if (p->finished) {
        VanargandPair_UpdatePairConditions(q);
        VanargandPair_ApplyPairTransition(q);
    }
}
