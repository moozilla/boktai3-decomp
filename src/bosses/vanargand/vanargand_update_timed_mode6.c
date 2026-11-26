#include "bosses/vanargand/vanargand_pair_attack.h"

void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);

void Vanargand_UpdateTimedMode6(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    if (Vanargand_ConsumeActionEntry(p) == 1)
        Vanargand_EnterAction(p, 6);
    if (p->finished) {
        ScenePanel_UpdatePairConditions(q);
        ScenePanel_ApplyPairTransition(q);
    }
    p->actionTimer++;
}
