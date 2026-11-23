#include "vanargand_pair_attack.h"

void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);

void Vanargand_UpdateVariantEntry(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    if (Vanargand_ConsumeActionEntry(p)) {
        if (q->mode == 0)
            Vanargand_EnterAction(p, 16);
        else
            Vanargand_EnterAction(p, 13);
    }
    if (p->finished) {
        if (p->state == 12) {
            ScenePanel_UpdatePairConditions(q);
            ScenePanel_ApplyPairTransition(q);
        } else {
            Vanargand_EnterAction(p, 12);
        }
    }
    p->actionTimer++;
}
