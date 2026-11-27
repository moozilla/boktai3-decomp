#include "global.h"
#include "bosses/vanargand/vanargand_pair_state_layout.h"
/* Paired Vanargand actor. Field names follow observed state and sprite operations. */
s32 ScenePair_CountPositiveCounters(void *);
void VanargandPair_AdvancePairState(struct VanargandPairState *panel)
{
    s32 count;
    switch (panel->state) {
        case 0:
            count = ScenePair_CountPositiveCounters(panel->pair);
            if (count == 2)
                panel->state = 1;
            else if (count == 0)
                panel->state = 2;
            break;
        case 2:
            count = ScenePair_CountPositiveCounters(panel->pair);
            if (count == 0) {
                panel->state = 12;
                break;
            }
            goto reset;
        case 11:
            panel->state = 2;
            break;
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 12:
        default:
        reset:
            panel->state = 0;
            break;
    }
}
