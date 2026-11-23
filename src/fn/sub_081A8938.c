#include "vanargand_pair_attack.h"

void Vanargand_UpdateQueuedActionTransition(void *object)
{
    struct VanargandPairAttack *p = object;
    struct VanargandPairAttack *q = object;
    if (Vanargand_ConsumeActionEntry(p) && p->state != 22)
        Vanargand_EnterAction(p, 22);
    if (p->finished) {
        if (q->transition <= 2 && q->stageQueued[q->transition] && !q->part.breaks) {
            Vanargand_StartAction(p, 2, 0, 8);
        } else {
            Vanargand_StartAction(p, 1, 0, 2);
            Vanargand_EnterAction(p, 23);
        }
    } else {
        p->actionTimer++;
    }
}
