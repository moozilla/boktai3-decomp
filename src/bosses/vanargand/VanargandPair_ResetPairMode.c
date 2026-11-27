#include "bosses/vanargand/vanargand_pair_attack.h"

void VanargandPair_UpdatePairConditions(void *);
void VanargandPair_ApplyPairTransition(void *);

void VanargandPair_ResetPairMode(struct VanargandPairAttack *object)
{
    struct VanargandPairAttack *panel = object;
    if (Vanargand_ConsumeActionEntry(panel))
        Vanargand_EnterAction(panel, 16);
    if (panel->finished) {
        object->mode = 0;
        VanargandPair_UpdatePairConditions(object);
        VanargandPair_ApplyPairTransition(object);
    }
}
