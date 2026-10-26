#include "global.h"
#include "game_state.h"
void GameState_MoveSlotEntry(s32 value, s32 slot)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (*(s16 *)((u8 *)gGameState + 2 * i + 0x90) == value)
            *(s16 *)((u8 *)gGameState + 2 * i + 0x90) = -1;
    }
    *(s16 *)((u8 *)gGameState + 2 * slot + 0x90) = value;
}
