#include "global.h"
#include "system/game_state/game_state.h"
s32 SolarFruit_GetInventorySlot(u32 i)
{
    return *(i + gGameState->unk828);
}
