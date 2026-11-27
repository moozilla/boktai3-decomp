#include "global.h"

void Vanargand_AdvanceEffectStageThree(u8 *p)
{
    if (p[0x15] == 3)
        p[0x15] = 4;
}
