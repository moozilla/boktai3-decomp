#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

void Script_Call(u32, void *);

void VanargandPair_RunSelectionScript(struct VanargandPairAttack *boss)
{
    struct VanargandPairAttack *self = boss;

    if (Vanargand_ConsumeActionEntry(self)) {
        u8 animation = 30;

        self->requestedAnimation = animation;
    }
    if (self->finished) {
        Script_Call(boss->stageQueued[boss->transition], 0);
        if (boss->transition <= 2)
            boss->transition++;
    }
}
