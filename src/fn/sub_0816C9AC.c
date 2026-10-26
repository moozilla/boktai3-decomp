#include "global.h"
extern u8 *gGameState;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void GameState_SetWorldMapFlagFromScript(void)
{
    s32 n;
    u32 *p;
    if (Script_SeekToKeyword(0x69) == 0)
        return;
    n = Script_GetValue();
    p = (u32 *)(gGameState + 0x5bc);
    *p |= 1 << n;
}
