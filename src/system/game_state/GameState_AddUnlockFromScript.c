#include "global.h"
#include "engine/engine.h"
void GameState_AddUnlock(s32);
s32 GameState_AddUnlockFromScript(void)
{
    if (Script_SeekToKeyword('w')) {
        GameState_AddUnlock(Script_GetValue());
        return 1;
    }
    return 0;
}
