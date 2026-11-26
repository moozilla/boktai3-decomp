#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_SetPendingCue(struct VanargandPairAttack *boss, u16 cue)
{
    u32 mask = 0x200000;
    boss->flags |= mask;
    boss->pendingCue = cue;
}
