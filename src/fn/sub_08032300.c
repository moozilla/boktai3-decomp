#include "global.h"
#include "game_state.h"
int TextState_GetPairSpeakerLabel(void)
{
    switch (gGameState->form) {
        case 0:
            return 31;
        case 1:
        case 6:
            return 32;
        default:
            return 32;
    }
}
