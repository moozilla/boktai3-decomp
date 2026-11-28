#include "global.h"
#include "system/game_state/game_state.h"
void SolarFruit_ClearEquippedSlot(u32 i)
{
    (&gGameState->slots88[4])[i] = -1;
}
