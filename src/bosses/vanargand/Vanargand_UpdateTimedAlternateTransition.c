#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_UpdateTimedAlternateTransition(void *object)
{
    struct VanargandPairAttack *p = object;
    if (Vanargand_ConsumeActionEntry(p))
        Vanargand_EnterAction(p, 20);
    if (p->finished && p->actionTimer > 120) {
        if (!((struct VanargandPairAttack *)object)->variant) {
            Vanargand_StartAction(p, 0, 0, 13);
            Vanargand_EnterAction(p, 1);
        } else {
            Vanargand_StartAction(p, 0, 0, 18);
            Vanargand_EnterAction(p, 19);
        }
    } else {
        p->actionTimer++;
    }
}
