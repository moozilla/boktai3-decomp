#include "global.h"
#include "game_state.h"
s32 GameState_GetUnlockEntry(s32 index);
s32 GameState_HasUnlockedInventoryEntry(s32 value)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (((s16 *)((u8 *)gGameState + 0x78))[i] == value && GameState_GetUnlockEntry(value) > 0)
            return 1;
    }
    return 0;
}
