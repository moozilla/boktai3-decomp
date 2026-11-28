#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/engine.h"
void SolarFruit_SetInventorySlot(s32 slot, s32 value);
void GameState_MoveSlotEntry(s32 value, s32 slot);
/* Registry routine FE88 (Solar Fruit): clears the eight entries set by
   SolarFruit_SetInventorySlot, the eight slots at +0x90 and +0x54, then -n count, -w count
   values for SolarFruit_SetInventorySlot and -q eight values for GameState_MoveSlotEntry. */
void SolarFruit_Command(void)
{
    s32 i, count;
    for (i = 0; i <= 7; i++)
        SolarFruit_SetInventorySlot(i, -1);
    for (i = 0; i <= 7; i++)
        (&gGameState->slots88[4])[i] = -1;
    gGameState->actionAuxiliarySlot = 0;
    if (Script_SeekToKeyword('n'))
        count = Script_GetValue();
    else
        count = 0;
    if (count > 0) {
        if (Script_SeekToKeyword('w'))
            for (i = 0; i < count; i++)
                SolarFruit_SetInventorySlot(i, Script_GetValue());
        if (Script_SeekToKeyword('q'))
            for (i = 0; i <= 7; i++)
                GameState_MoveSlotEntry(Script_GetValue(), i);
    }
}
