#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/engine.h"
void GameState_AddEquipment(s32 value);
/* Script: clears the unlock mask (+0x4C), the four slots at +0x88 and the
   selection (+0x52), then -n count, -m count values for GameState_AddEquipment and -q
   four slot values. */
void GameState_ResetSlotsFromScript(void)
{
    s32 i, count;
    gGameState->unlockMask = 0;
    for (i = 0; i <= 3; i++)
        *(i + gGameState->slots88) = -1;
    gGameState->selected = 0;
    if (Script_SeekToKeyword('n'))
        count = Script_GetValue();
    else
        count = 0;
    if (count > 0) {
        if (Script_SeekToKeyword('m'))
            for (i = 0; i < count; i++)
                GameState_AddEquipment(Script_GetValue());
        if (Script_SeekToKeyword('q'))
            for (i = 0; i <= 3; i++)
                *(i + gGameState->slots88) = Script_GetValue();
    }
}
