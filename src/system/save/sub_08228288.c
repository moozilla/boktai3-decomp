#include "global.h"
u32 Script_GetValue(void);
extern u8 *gGameState;
void GameState_SetSavedSceneFromScript(void)
{
    u32 v = Script_GetValue();
    *(u32 *)(gGameState + 0x5a4) = v;
}
