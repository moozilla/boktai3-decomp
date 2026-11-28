#include "global.h"
#include "system/game_state/game_state.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void SolarBank_SetBalanceFromScript(void)
{
    if (Script_SeekToKeyword(0x61)) {
        s32 v = Script_GetValue();
        if (v > 9999)
            v = 9999;
        gGameState->unk7B0 = v;
    }
}
