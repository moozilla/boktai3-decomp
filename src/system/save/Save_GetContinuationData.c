#include "system/game_state/game_state.h"
u8 *Save_GetContinuationData(void)
{
    return (u8 *)gGameState + 0x778;
}
