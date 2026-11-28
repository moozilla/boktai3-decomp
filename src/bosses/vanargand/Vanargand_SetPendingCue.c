#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_SetPendingCue(struct VanargandPairAttack *boss, u16 cue)
{
    VanargandPairAttack_SetFlags(boss, 0x200000);
    boss->pendingCue = cue;
}
