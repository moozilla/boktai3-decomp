#include "global.h"
#include "system/game_state/game_state.h"
u32 GameState_HasEquipment(u32 i)
{
    return gGameState->unlockMask & (1 << i);
}
