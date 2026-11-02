#include "global.h"
#include "game_state.h"
static inline u8 TextState_HasPlayerFlag(u32 mask)
{

    if (mask & gGameState->flags868)
        return 1;
    return 0;
}

int TextState_GetPlayerSpeakerLabel(void)
{
    if (TextState_HasPlayerFlag(128))
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
