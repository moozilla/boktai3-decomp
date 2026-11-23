#include "vanargand_pair_attack.h"

void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);

void ScenePanel_ResetPairMode(struct VanargandPairAttack *object)
{
    struct VanargandPairAttack *panel = object;
    if (Vanargand_ConsumeActionEntry(panel))
        Vanargand_EnterAction(panel, 16);
    if (panel->finished) {
        object->mode = 0;
        ScenePanel_UpdatePairConditions(object);
        ScenePanel_ApplyPairTransition(object);
    }
}
