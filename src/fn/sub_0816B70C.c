#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
extern u8 *gGameState;
void GameState_SetAreaFromScript(void)
{
    if (Script_SeekToKeyword(0x73))
        *(u16 *)(gGameState + 0x5ac) = Script_GetValue();
}
