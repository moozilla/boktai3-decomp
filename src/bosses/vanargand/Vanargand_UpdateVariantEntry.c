#include "bosses/vanargand/vanargand_pair_attack.h"

void VanargandPair_UpdatePairConditions(void *);
void VanargandPair_ApplyPairTransition(void *);

void Vanargand_UpdateVariantEntry(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    if (Vanargand_ConsumeActionEntry(p)) {
        if (q->mode == 0)
            Vanargand_EnterAction(p, 16);
        else
            Vanargand_EnterAction(p, 13);
    }
    if (p->finished) {
        if (p->state == 12) {
            VanargandPair_UpdatePairConditions(q);
            VanargandPair_ApplyPairTransition(q);
        } else {
            Vanargand_EnterAction(p, 12);
        }
    }
    p->actionTimer++;
}
