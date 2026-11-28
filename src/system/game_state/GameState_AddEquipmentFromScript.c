#include "global.h"
#include "engine/engine.h"
void GameState_AddEquipment(s32);
void GameState_AddEquipmentFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        GameState_AddEquipment(Script_GetValue());
}
