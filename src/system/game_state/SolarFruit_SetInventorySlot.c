#include "global.h"
#include "system/game_state/game_state.h"
void SolarFruit_SetInventorySlot(u32 i, u16 v)
{
    *(i + gGameState->unk828) = v;
}
