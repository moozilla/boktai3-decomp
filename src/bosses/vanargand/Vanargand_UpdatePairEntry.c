#include "bosses/vanargand/vanargand_pair_attack.h"

u32 Trigger_ConsumePending(void *, u32);

void VanargandPair_UpdatePairConditions(void *);
void VanargandPair_ApplyPairTransition(void *);

void Vanargand_UpdatePairEntry(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    if (Vanargand_ConsumeActionEntry(p))
        Vanargand_EnterAction(p, 16);
    if (p->finished) {
        if (ScenePair_CountPositiveCounters(q->pair) <= 1) {
            Vanargand_EnterAction(p, 2);
        } else {
            VanargandPair_UpdatePairConditions(q);
            VanargandPair_ApplyPairTransition(q);
        }
    }
    p->actionTimer++;
}
