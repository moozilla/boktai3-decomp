#include "global.h"
extern u8 *gGameState;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void GameState_SetMapRevealFlagFromScript(void)
{
    if (Script_SeekToKeyword(0x69)) {
        s32 v = Script_GetValue();
        *(u32 *)(gGameState + 0x5B8) |= 1 << v;
    }
}
