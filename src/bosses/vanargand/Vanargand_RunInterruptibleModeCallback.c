#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

extern const u32 gUnk_08612004[];
void Callback_InvokeActor(void *, u32);
s32 VanargandPair_RequestPairReaction(struct VanargandPairAttack *);

void Vanargand_RunInterruptibleModeCallback(struct VanargandPairAttack *boss)
{
    if (VanargandPair_RequestPairReaction(boss) == 0)
        Callback_InvokeActor(boss, gUnk_08612004[boss->actionIndex]);
}
