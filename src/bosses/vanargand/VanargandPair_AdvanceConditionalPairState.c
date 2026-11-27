#include "global.h"
#include "bosses/vanargand/vanargand_pair_state_layout.h"

s32 ScenePair_CountPositiveCounters(void *);
void VanargandPair_AdvanceConditionalPairState(struct VanargandPairState *panel)
{
    s32 count;
    switch (panel->state) {
        case 3:
            count = ScenePair_CountPositiveCounters(panel->pair);
            if (count == 2)
                panel->state = 4;
            else if (count == 0)
                panel->state = 6;
            break;
        case 4:
            if (ScenePair_CountPositiveCounters(panel->pair) != 2)
                goto reset;
            {
                s32 t = panel->gauge;
                if (t <= 42)
                    panel->state = 8;
                else
                    panel->state = 7;
            }
            break;
        case 6:
            if (ScenePair_CountPositiveCounters(panel->pair) != 0)
                goto reset;
            panel->state = 12;
            break;
        case 7:
        case 8:
            if (ScenePair_CountPositiveCounters(panel->pair) != 2)
                goto reset;
            panel->state = 5;
            break;
        case 11:
            panel->state = 6;
            break;
        case 5:
        case 9:
        case 10:
        case 12:
        default:
        reset:
            panel->state = 3;
            break;
    }
}
