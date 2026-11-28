#include "global.h"
#include "engine/engine.h"
#include "system/game_state/game_state.h"
void GameState_ClearConsumableSlot(s32 slot);
void Inventory_SetConsumableEntry(s32 slot, s32 value);
/* Script: clears game state flags720/724, the 48 entries of table160 and
   party[4], then refills them: -n count, -a count values for table160,
   -e four party ids. */
void GameState_ResetListsFromScript(void)
{
    s32 i, count;
    for (i = 0; i <= 1; i++)
        (&gGameState->flags720)[i] = 0;
    for (i = 0; i <= 47; i++)
        GameState_ClearConsumableSlot(i);
    for (i = 0; i <= 3; i++)
        *(i + gGameState->party) = -1;
    if (Script_SeekToKeyword('n'))
        count = Script_GetValue();
    else
        count = 0;
    if (count > 0) {
        if (Script_SeekToKeyword('a'))
            for (i = 0; i < count; i++)
                Inventory_SetConsumableEntry(i, Script_GetValue());
        if (Script_SeekToKeyword('e'))
            for (i = 0; i <= 3; i++)
                *(i + gGameState->party) = Script_GetValue();
    }
}
