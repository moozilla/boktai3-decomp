#define GameState_SetListEntry SolarFruit_SetInventorySlot
#include "global.h"
#include "game_state.h"
void SolarFruit_SetInventorySlot(s32, s32);
s32 GameState_InsertUniqueListEntry(s32 value)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (gGameState->unk828[i] == value)
            return 0;
    }
    for (i = 0; i < 8; i++) {
        if (*(s16 *)((u8 *)gGameState + i * 2 + 0x828) == -1) {
            SolarFruit_SetInventorySlot(i, value);
            return 1;
        }
    }
    return 0;
}
