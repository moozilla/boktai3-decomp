#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_ResetPaletteTransition(struct VanargandPairAttack *boss)
{
    if (boss->paletteMode != 0) {
        boss->paletteFrame = 0;
        boss->paletteMode = 0;
        boss->paletteSelector = 0x6F;
    }
}
