// Tentative Vanargand ownership; field roles follow the mode transition accesses.
#include "vanargand_pair_attack.h"

void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);

void Vanargand_UpdateModeCycle(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    if (Vanargand_ConsumeActionEntry(p)) {
        if (p->mode == 1)
            Vanargand_EnterAction(q, 16);
        else
            Vanargand_EnterAction(q, 7);
        q->actionTimer = 0;
    }
    if (q->finished) {
        if (q->actionTimer == 0 && q->state == 16) {
            Vanargand_EnterAction(q, 7);
        } else if (q->state == 7) {
            Vanargand_EnterAction(q, 8);
        } else if (q->state == 8) {
            Vanargand_EnterAction(q, 16);
        } else {
            ScenePanel_UpdatePairConditions(p);
            ScenePanel_ApplyPairTransition(p);
        }
        q->actionTimer++;
    }
}
