#include "bosses/vanargand/vanargand_pair_attack.h"

void ScenePanel_UpdateActiveTransition(struct VanargandPairAttack *panel)
{
    if (Vanargand_ConsumeActionEntry(panel))
        Vanargand_EnterAction(panel, 14);
    if (panel->finished)
        Vanargand_EnterActionPhase(panel, 3);
    panel->actionTimer++;
}
