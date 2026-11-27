#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
extern u8 *gGameState;
void GameState_SetFloorFromScript(void)
{
    if (Script_SeekToKeyword(0x66))
        *(u16 *)(gGameState + 0x5ae) = Script_GetValue();
}
