#include "bosses/vanargand/vanargand_pair_attack.h"

void VanargandPair_UpdatePairConditions(void *);
void VanargandPair_ApplyPairTransition(void *);

void Vanargand_UpdateMode9Transition(void *object)
{
    struct VanargandPairAttack *p = object;
    if (Vanargand_ConsumeActionEntry(p)) {
        if (!((struct VanargandPairAttack *)object)->variant) {
            Vanargand_EnterAction(p, 5);
            return;
        } else {
            Vanargand_EnterAction(p, 9);
        }
    } else if (p->finished) {
        if (VanargandPairAttack_TestFlags(p, 0x20000)) {
            Vanargand_EnterActionPhase(p, 16);
        } else {
            VanargandPair_UpdatePairConditions(object);
            VanargandPair_ApplyPairTransition(object);
        }
    }
    p->actionTimer++;
}
