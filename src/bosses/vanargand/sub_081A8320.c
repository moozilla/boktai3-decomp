#include "bosses/vanargand/vanargand_pair_attack.h"

void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);

void ScenePanel_UpdateActivePairState(void *object)
{
    struct VanargandPairAttack *panel = object;
    if (Vanargand_ConsumeActionEntry(panel)) {
        Vanargand_EnterAction(panel, 11);
        panel->actionTimer = 0;
    }
    if (panel->finished) {
        ScenePanel_UpdatePairConditions(object);
        ScenePanel_ApplyPairTransition(object);
    }
}
