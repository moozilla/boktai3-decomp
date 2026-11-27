#include "global.h"

void Vanargand_AdvanceEffectStageOne(u8 *p)
{
    if (p[0x15] == 1)
        p[0x15] = 2;
}
