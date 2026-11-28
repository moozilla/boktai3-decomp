#include "global.h"
#include "engine/engine.h"
void GameState_InsertUniqueListEntry(s32);
s32 SolarFruit_AddFromScript(void)
{
    if (Script_SeekToKeyword('w')) {
        GameState_InsertUniqueListEntry(Script_GetValue());
        return 1;
    }
    return 0;
}
