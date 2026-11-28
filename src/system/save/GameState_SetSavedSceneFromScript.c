#include "system/game_state/game_state.h"
u32 Script_GetValue(void);
void GameState_SetSavedSceneFromScript(void)
{
    gGameState->unk5A4 = Script_GetValue();
}
