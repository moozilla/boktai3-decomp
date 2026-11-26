#include "bosses/vanargand/vanargand_pair_attack.h"

void VanargandPair_UpdatePairConditions(void *);
void VanargandPair_ApplyPairTransition(void *);

void VanargandPair_UpdateActivePairState(void *object)
{
    struct VanargandPairAttack *panel = object;
    if (Vanargand_ConsumeActionEntry(panel)) {
        Vanargand_EnterAction(panel, 11);
        panel->actionTimer = 0;
    }
    if (panel->finished) {
        VanargandPair_UpdatePairConditions(object);
        VanargandPair_ApplyPairTransition(object);
    }
}
