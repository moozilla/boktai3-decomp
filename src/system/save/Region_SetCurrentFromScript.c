#include "system/game_state/game_state.h"
#include "engine/ram.h"
s32 Script_GetValue(void);
void Region_SetCurrentFromScript(void)
{
    {
        s32 region = Script_GetValue();

        gGameState->region = region;
    }
    gRegionOverride = 1;
}
