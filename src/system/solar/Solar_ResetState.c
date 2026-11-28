#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
#include "engine/runtime.h"
/* Solar_ResetState: Clears the solar task pointer and current and cached intensity levels */
void Solar_ResetState(void)
{
    gSolarController = 0;
    *(u16 *)&gGameState->unk874 = 0;
    *(u16 *)&gGameState->unk876 = 0;
    gSolarSavedLevel = 0;
    gSolarSavedCachedLevel = 0;
    gSolarPaused = 0;
}
