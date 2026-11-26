#include "bosses/vanargand/vanargand_pair_attack.h"

// confidence=low: enters state 25 and requests mode 13 when active.
void Vanargand_UpdateMode25Transition(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    if (Vanargand_ConsumeActionEntry(p))
        Vanargand_EnterAction(p, 25);
    if (p->finished) {
        q->attackPhase = 0;
        Vanargand_StartAction(p, 0, 0, 13);
    } else {
        p->actionTimer++;
    }
}
