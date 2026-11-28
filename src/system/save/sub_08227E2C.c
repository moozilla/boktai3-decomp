#include "global.h"
#include "system/game_state/game_state.h"
void sub_08227E2C(s32 index)
{
    if ((s16)(&gGameState->counter87C)[index] < 9999)
        (&gGameState->counter87C)[index]++;
}
