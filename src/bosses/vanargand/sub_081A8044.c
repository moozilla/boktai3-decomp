#include "bosses/vanargand/vanargand_pair_attack.h"

void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);

void ScenePanel_UpdateDelayedPairState(void *object)
{
    struct VanargandPairAttack *panel = object;
    if (Vanargand_ConsumeActionEntry(panel))
        Vanargand_EnterAction(panel, 1);
    if (panel->actionTimer > 128) {
        ScenePanel_UpdatePairConditions(object);
        ScenePanel_ApplyPairTransition(object);
    }
    panel->actionTimer++;
}
