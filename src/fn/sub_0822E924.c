#include "global.h"
#include "engine.h"
void GameState_SetUnlockSlot(s32 index, s32 value);
void GameState_AddUnlock(s32 value);
void Inventory_AssignUniqueSecondarySlot(s32 value, s32 slot);
/* game state fields, addressed index-first as the original does */
#define STATE_U16(offset) (*(u16 *)((u8 *)gGameState + (offset)))
#define SLOT78(i)         (*(s16 *)((u8 *)gGameState + (i) * 2 + 0x78))
/* Script: clears the twelve entries set by GameState_SetUnlockSlot, the eight slots at
   +0x78 and +0x50, then -n count, -w count values (nonzero ones go to
   GameState_AddUnlock) and -q eight values for Inventory_AssignUniqueSecondarySlot (+0x50 = 8). */
void GameState_ResetUnlocksFromScript(void)
{
    s32 i, count;
    for (i = 0; i <= 11; i++)
        GameState_SetUnlockSlot(i, 0);
    for (i = 0; i <= 7; i++)
        SLOT78(i) = -1;
    STATE_U16(0x50) = 0;
    if (Script_SeekToKeyword('n'))
        count = Script_GetValue();
    else
        count = 0;
    if (count > 0) {
        if (Script_SeekToKeyword('w')) {
            for (i = 0; i < count; i++) {
                s32 value = Script_GetValue();
                if (value)
                    GameState_AddUnlock(value);
            }
        }
        if (Script_SeekToKeyword('q')) {
            STATE_U16(0x50) = 8;
            for (i = 0; i <= 7; i++)
                Inventory_AssignUniqueSecondarySlot(Script_GetValue(), i);
        }
    }
}
