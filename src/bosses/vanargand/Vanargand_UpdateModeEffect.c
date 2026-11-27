#include "global.h"
#include "bosses/vanargand/vanargand_pair_state_layout.h"

void VanargandPair_AdvancePairState(void *);
void VanargandPair_AdvanceConditionalPairState(void *);

void Vanargand_UpdateModeEffect(struct VanargandPairState *p)
{
    switch (p->level) {
        case 1:
            VanargandPair_AdvancePairState(p);
            break;
        case 2:
        case 3:
            VanargandPair_AdvanceConditionalPairState(p);
            break;
    }
}
