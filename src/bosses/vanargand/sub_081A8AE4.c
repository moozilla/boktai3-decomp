#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_UpdateMode24Transition(struct VanargandPairAttack *panel)
{
    if (Vanargand_ConsumeActionEntry(panel))
        Vanargand_EnterAction(panel, 24);
    panel->actionTimer++;
}
