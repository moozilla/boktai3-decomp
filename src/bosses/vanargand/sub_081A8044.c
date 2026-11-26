#include "bosses/vanargand/vanargand_pair_attack.h"

void VanargandPair_UpdatePairConditions(void *);
void VanargandPair_ApplyPairTransition(void *);

void VanargandPair_UpdateDelayedPairState(void *object)
{
    struct VanargandPairAttack *panel = object;
    if (Vanargand_ConsumeActionEntry(panel))
        Vanargand_EnterAction(panel, 1);
    if (panel->actionTimer > 128) {
        VanargandPair_UpdatePairConditions(object);
        VanargandPair_ApplyPairTransition(object);
    }
    panel->actionTimer++;
}
