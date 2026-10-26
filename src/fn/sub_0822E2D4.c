#include "global.h"
#include "game_state.h"
void GameState_UnequipConsumableSlot(s32 a)
{
    s32 i = 0;
    do {
        u8 *g = gGameState;
        s16 *q = (s16 *)(i * 2 + (u32)g + 0x58);
        if (*q == a)
            *q = -1;
        i++;
    } while (i <= 3);
}
