// COMPILER: old_agbcc
#include "global.h"
#include "system/game_state/game_state.h"
/* SolarBank_GetBalance: Returns the saved balance also used by SolarBank_ApplyInterestFromScript */

s16 SolarBank_GetBalance(void)
{
    return gGameState->unk7B0;
}
