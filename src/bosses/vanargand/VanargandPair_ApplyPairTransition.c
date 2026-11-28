#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

void VanargandPair_ApplyPairTransition(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    switch (q->pairTransition - 1) {
        case 0:
            if (ScenePair_CountPositiveCounters(q->pair) == 2) {
                if (q->pairLevel == 3) {
                    Vanargand_EnterActionPhase(p, 6);
                    break;
                }
                if (q->pairLevel > 1) {
                    Vanargand_EnterActionPhase(p, 5);
                    break;
                }
            }
            Vanargand_EnterActionPhase(p, 4);
            break;
        case 1:
            if (q->mode == 0) {
                Vanargand_EnterActionPhase(p, 12);
                return;
            }
            Vanargand_EnterActionPhase(p, 3);
            break;
        case 2:
            if (q->pairLevel > 1)
                Vanargand_EnterActionPhase(p, 8);
            else
                Vanargand_EnterActionPhase(p, 7);
            break;
        case 3:
            Vanargand_EnterActionPhase(p, 9);
            break;
        case 4:
            if (ScenePair_CountPositiveCounters(q->pair) > 0)
                Vanargand_EnterActionPhase(p, 10);
            else
                Vanargand_EnterActionPhase(p, 18);
            break;
        case 6:
            Vanargand_EnterActionPhase(p, 2);
            break;
        case 7:
            Vanargand_EnterActionPhase(p, 15);
            break;
        case 5:
            Vanargand_EnterActionPhase(p, 13);
            break;
        case 9:
            Vanargand_EnterActionPhase(p, 14);
            break;
        case 8:
            Vanargand_EnterActionPhase(p, 1);
            return;
        default:
            return;
    }
}
