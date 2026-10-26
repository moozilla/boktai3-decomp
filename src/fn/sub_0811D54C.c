#include "global.h"
#include "game_state.h"
void GameState_ResetChallengeRecords(void)
{
    s32 i = 0;
    do {
        u8 *g = gGameState;
        *(s32 *)(g + i * 4 + 0x7c8) = -1;
        *(s16 *)(g + i * 2 + 0x800) = -1;
        i++;
    } while (i <= 11);
}
