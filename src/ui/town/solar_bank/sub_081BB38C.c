// COMPILER: old_agbcc
#include "global.h"
/* SolarBank_GetBalance: Returns the saved balance also used by SolarBank_ApplyInterestFromScript */

extern u8 *gGameState;

s16 SolarBank_GetBalance(void)
{
    return *(s16 *)(gGameState + 0x7b0);
}
