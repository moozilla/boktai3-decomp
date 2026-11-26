#include "engine/shared/global.h"
#include "engine/shared/game_state_flags.h"

int TextState_GetPlayerSpeakerLabel(void)
{
    if (GameState_TestWorldFlags(128))
        return 28;
    switch (gGameState->form) {
        case 0:
            return 0;
        case 1:
            return 0;
        case 6:
            return 4;
        default:
            return 0;
    }
}
