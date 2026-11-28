#include "system/game_state/game_state.h"
u8 *Save_GetContinuationData(void);
s32 Save_GetMainStateSize(void)
{
    return Save_GetContinuationData() - (u8 *)gGameState;
}
