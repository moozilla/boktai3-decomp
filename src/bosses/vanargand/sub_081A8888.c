#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_UpdateTimedAlternateTransition(void *object)
{
    struct VanargandPairAttack *p = object;
    if (Vanargand_ConsumeActionEntry(p))
        Vanargand_EnterAction(p, 20);
    if (p->finished && p->actionTimer > 120) {
        const struct VanargandPairAttack *source = object;
        if (!source->variant) {
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
