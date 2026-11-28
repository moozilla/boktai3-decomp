#include "global.h"
#include "system/game_state/game_state.h"
void GameState_ResetChallengeRecords(void)
{
    s32 i;

    for (i = 0; i < 12; i++) {
        *(i + gGameState->unk7C8) = -1;
        *(i + gGameState->unk800) = -1;
    }
}
