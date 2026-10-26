#include "global.h"
#include "engine.h"
/* Script: clears the flag word of area -s at game state +0x5C0, or all
   fifteen when -s is absent (counterpart of GameState_SetAreaFlagFromScript). */
void GameState_ClearAreaFlagsFromScript(void)
{
    s32 i;
    if (Script_SeekToKeyword('s')) {
        i = Script_GetValue();
        *(u32 *)((u8 *)gGameState + i * 4 + 0x5C0) = 0;
    } else {
        for (i = 0; i <= 14; i++)
            *(u32 *)((u8 *)gGameState + i * 4 + 0x5C0) = 0;
    }
}
